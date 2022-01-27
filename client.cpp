#include <stdio.h>      /* Contains common I/O functions */
#include <sys/types.h>  /* Contains definitions of data types used in system calls */
#include <sys/socket.h> /* Includes definitions of structures needed for sockets */
#include <netinet/in.h> /* Contains constants and structures needed for internet domain addresses. */
#include <unistd.h>     /* Contains standard unix functions */
#include <stdlib.h>     /* For atoi() and exit() */
#include <string.h> 	/* For memset() */
#include <arpa/inet.h>  /* For inet_pton() */
#include <time.h>       /* For retrieving the time */
#include <limits.h>	/* For the maximum integer size */
#include <sys/stat.h>   /* For stat() */
#include "TCPLib.h"     /* Defines tcp_recv, tcp_send, tcp_recv_size, tcp_recv_size */
#include <sys/sendfile.h> /* For sendfile() */
#include <fcntl.h>        /* For O_RDONLY */
#include <iostream>
#include <string>
using namespace std;
#define MAXSIZE 1024

/**
 * Returns the file size 
 * @param fileName - the file name
 * @return - the corresponding file size
 */

int getFileSize(const char* fileName)
{
	/* The structure containing the file information */
	struct stat fileInfo;
	
	/* Get the file size */
	if(stat(fileName, &fileInfo) < 0)
		return -1;
	
	/* Return the file size */
	return fileInfo.st_size;
}

/**
 * Transmits the file name
 * @param socket - the socket to send the file name over
 * @param fileName - the file name to send
 */
void sendFileInfo(const int& socket, const char* fileName)
{
	/* Get the size of the file name */
	int fileNameSize = strlen(fileName);
	
	/* Send the size of the file name */
	if(tcp_send_size(socket, fileNameSize) < 0)
	{
		perror("tcp_send_size");
		exit(-1);
	}
	
	/* Send the actual file name */	
	if(tcp_send(socket, fileName, fileNameSize) < 0)
	{
		perror("tcp_send");
		exit(-1);
	}
}

FILE *popen(const char *command, const char *type);

/**
 * Returns the output of a command in C++ string format
 * @param cmd - the command to execute
 * @return - the output of the commaned in C++ format
 */
string getCmdOutput(const char* cmd)
{
	/* The buffer to store the results */
	string result = "";
	
	/* Open the output stream of the program for reading */
	FILE* fp = popen(cmd, "r");
	
	/* The output buffer */
	char outBuff[10];
	
	/* The number of bytes read */
	int numRead = 0;
	
	/* Make sure the stream was opened */
	if(!fp)
	{
		perror("popen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{	
		/* Read information from the output stream of the program */
		if((numRead = fread(outBuff, sizeof(char), sizeof(outBuff) - 1, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
			
		/* NULL terminate the string */
		outBuff[numRead] = '\0';	
	
		/* If anything was read, then save it! */
		if(numRead)
			result += outBuff;
		
	}
	
	/* Close the output stream of the program */
	if(pclose(fp) < 0)
	{
		perror("pclose");
		exit(-1);
	}
	
	return result;
	
}

int main(int argc, char** argv)
{
	/* The port number */	
	int port = -1;
	
	/* The file descriptor representing the connection to the client */
	int connfd = -1;
	
	/* The number of bytes sent in one shot */
	int numSent = 0;
	
	/* The total number of bytes sent */
	off_t totalNumSent = 0;
	
	/* The size of the file name */
	int fileNameSize = -1;

	/* The name of the file */
	const char* fileName;
		
	/* The structures representing the server address */
	sockaddr_in serverAddr;
	
	/* Stores the size of the client's address */
	socklen_t servLen = sizeof(serverAddr);	
	
	
	/* Make sure the user has provided the port number to
	 * listen on
	 */
	if(argc < 3)
	{
		/* Report an error */
		fprintf(stderr, "USAGE: %s <SERVER IP> <SERVER PORT #> ", argv[0]);
		exit(-1);	
	}
	
	/* Get the port number */
	port = atoi(argv[2]);
	
	/* Get the file name */
	//const char* fileName = argv[3];
	printf("ftp>");

	//vars for client input
	string command;			//get,put,ls,quit
	string nameFile;		//if get or put inputted for command, second input expected
	string input = "";

	//takes in user input
	cin >> command;

//peeks to check if they inputted a file name after command
if (cin.peek() == '\n')
{
	//if they only inputted one word, it is either ls or quit
}

//if '/n' isn't reached, we assume it is either get or put
//so we take in nameFile
else
{
    cin >> nameFile;
}
	fileName = nameFile.c_str();
		
	/* Make sure that the port is within a valid range */
	if(port < 0 || port > 65535)	
	{
		fprintf(stderr, "Invalid port number\n");
		exit(-1);
	} 
	
	/* Connect to the server */
	if((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(-1);
	}
	
		
	/* Set the structure to all zeros */
	memset(&serverAddr, 0, sizeof(serverAddr));
		
	/* Set the server family */
	serverAddr.sin_family = AF_INET;
	
	/* Convert the port number to network representation */	
	serverAddr.sin_port = htons(port);
	
	
	/**
	 * Convert the IP from the presentation format (i.e. string)
	 * to the format in the serverAddr structure.
	 */
	if(!inet_pton(AF_INET, argv[1], &serverAddr.sin_addr))
	{
		perror("inet_pton");
		exit(-1);
	}
	
	
	/* Lets connect to the client. This call will return a socket used 
	 * used for communications between the server and the client.
	 */
	if(connect(connfd, (sockaddr*)&serverAddr, sizeof(sockaddr))<0)
	{
		perror("connect");
		exit(-1);
	}	
	send(connfd, command.c_str(), MAXSIZE, 0);
	while(command != "quit")
	{
		if(command == "put")
		{
			/* Send the file name */
			sendFileInfo(connfd, fileName);
			
			/* Get the file size */
			int fileSize = getFileSize(fileName);	
			
			/* Did we successfully obtain the file size ? */
			if(fileSize < 0)
			{
				perror("stat");
				exit(-1);
			}
				
			/* Send the file size */
			if(tcp_send_size(connfd, fileSize) < 0)
			{
				perror("tcp_send_size");
				exit(-1);
			}
			
			/* Open the file */
			int fd = open(fileName, O_RDONLY);
			
			/* Make sure the file was successfully opened */
			if(fd < 0)
			{
				perror("fopen");
			}	
			
			
			/* Keep sending until the whole file is sent */	
			while(totalNumSent < fileSize)
			{	
				/* Send the file to the server */
				if((numSent = sendfile(connfd, fd, &totalNumSent, fileSize - totalNumSent)) < 0)
				{
					perror("sendfile");
					exit(-1);
				}
				
				/* Update the total number of bytes sent */
				totalNumSent += numSent;
						
			}	
			/* Close the file */
			close(fd);	
		}
		else if (command == "ls")
		{
			cout << endl << "Files on the server:" << endl;
			/* Get the output of the ls command */
			string result =  getCmdOutput("ls");
				
			/* Print the output of the ls command */
			fprintf(stderr, "%s\n", result.c_str());
		}
		else if (command == "get")
		{
			
			/* Send the file name */
			sendFileInfo(connfd, fileName);
			int fileSize = 0;
			char recvBuff[MAXSIZE];
			int numRead = 0;
			/* Get the file size */
				if((fileSize = tcp_recv_size(connfd)) < 0)
				{
					perror("tcp_recv_size");
					exit(-1);
				}
				
					
				fprintf(stderr, "Receiving file: %s (%d bytes)...\n", fileName, fileSize);	
					
				/* Open the file */
				FILE* fp;
				fp = fopen(fileName, "w");

				/* Error checks */
				if(!fp)
				{
					perror("fopen");
					exit(-1);
				}
				
				/* Keep receiving until the whole file is received */
				while(fileSize != 0)
				{
					/* Receive the chunk */
					if((numRead = tcp_recv(connfd, recvBuff, min(MAXSIZE, fileSize))) < 0)
					{
						perror("tcp_recv");
						exit(-1);
					}	
					/* Save the received buffer */
					if(fwrite(recvBuff, sizeof(char), numRead, fp) < 0)
					{
						perror("fwrite");
						exit(-1);
					}
					
					/* Reduce the file size */
					fileSize -= numRead;
					
				}
				
				fprintf(stderr, "File %s succesfully received.\n", fileName);
				/* Close the file */
				fclose(fp);
		}
		printf("\nftp>");
		cin >> command;
		if (cin.peek() == '\n')
		{

		}
		else
		{
			cin >> nameFile;
		}
		fileName = nameFile.c_str();
		send(connfd, command.c_str(), MAXSIZE, 0);
	}
	/* Close the connection socket */	
	close(connfd);		
	return 0;
}





