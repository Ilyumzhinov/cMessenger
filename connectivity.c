#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for malloc(), exit() */
#include <unistd.h> /* for close() */
#include <string.h> /* for strcopy(), strlen(), memset() */
#include <arpa/inet.h> /* for struct sockaddr_in, SOCK_STREAM */

#define PORT 8080
#define MAXSIZE 1024
/* Max participants waiting for a connection */
#define MAXPENDING 1

char buffer[MAXSIZE];

int SendMsg(int);
int ReceiveMsg(int);
/**/

/*FUNCTIONS*/
/* Create a listening host using a socket */
/* Source code: 
 https://www.geeksforgeeks.org/socket-programming-cc/
 https://codereview.stackexchange.com/questions/13461/two-way-communication-in-tcp-server-client-implementation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
 http://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf */
int CreateServer()
{
	  /* Socket descriptor for serverAddress */
    int serverSocket;
    /* Socket descriptor for client */
    int clientSocket;
    /* Local address:
     struct sockaddr_in - data type specific to TCP/IP addresses */
    struct sockaddr_in serverAddress;
    /* Client address */
    struct sockaddr_in clientAddress;
    unsigned int size = sizeof(clientAddress);
    
    int messageSize;
    /**/
    
    /* SOCKET: Create socket for incoming connections.
     AF_INET - IPv4 protocols family,
     SOCK_STREAM - TCP socket type,
     0 - default protocol */
    if (-1 == (serverSocket = socket(AF_INET, SOCK_STREAM, 0)))
    {
        AddMessage(SYSTEMUSER, "Socket failure!", 0);
    }
    
    /* Construct local address structure */
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    serverAddress.sin_port = htons(PORT); /* Local port */
    
    /* BIND: Assign address to socket.
     serverSocket - socket descriptor,
     serverAddress - the address and port,
     size - size of sockaddr_in structure */
    if (-1 == (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))))
    {
        AddMessage(SYSTEMUSER, "Binding Failure!", 0);
        close(serverSocket);
        return -1;
    }
    
    /* LISTEN: Mark the socket to listen for incoming connections */
    if (-1 == listen(serverSocket, MAXPENDING))
    {
        AddMessage(SYSTEMUSER, "Listening Failure!", 0);
        close(serverSocket);
        return -1;
    }
    
    PrintMessage(SYSTEMUSER, "Waiting for a connection...", 0);
    
    /* Allow multiple consecutive connections to a listening socket */
    for(;;)
    {
        /* ACCEPT: Dequeue the net connection on the socket (blocking).
         clientSocket - socket used for data transfer,
         serverSocket - being listened to socket,
         &clientAddress - address of the active participant (filled in upon return),
         size - size of sockaddr_in structure */
        if (-1 == (clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &size)))
        {
            AddMessage(SYSTEMUSER, "Accept error!", 0);
            close(serverSocket);
            return -1;
        }
        
        {
            AddMessage(SYSTEMUSER, "Connection established! Say hello;)", 0);
        }
        
        /* Allow multiple send/receives */
        while(1)
        {
            /* Send a message */
            if (-1 == SendMsg(clientSocket))
              break;
          
            /* Receive a message */
            if (-1 == ReceiveMsg(clientSocket))
              break;
          
            AddMessage(SYSTEMUSER, buffer, 0);
        }
        
        /* CLOSE: Close connection, free port used */   
        close(clientSocket);
        
        AddMessage(SYSTEMUSER, "Connection closed!", 0);
    }
    return 0;
}

/* Create a client by connecting to a listening socket at a specified IP address */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/
 https://codereview.stackexchange.com/questions/13461/two-way-communication-in-tcp-server-client-implementation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
 http://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf */
int CreateClient()
{
    /* Socket data */
    int clientSocket;
    struct sockaddr_in serverAddress;
    
    char ipServer[64];
    /* To print Enter IP only once */
    int mem = 0;
    int messageSize;
    /**/
    
    /* SOCKET: Create an active TCP socket */
    if (-1 == (clientSocket = socket(AF_INET, SOCK_STREAM, 0)))
    {
        AddMessage(SYSTEMUSER, "Socket creation error!", 0);
        return -1;
    }
    
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_port = htons(PORT); /* Server port */
    
    /* Receive a server IP address from a user.
     Convert IPv4 and IPv6 addresses from text to binary form */
    while (0 >= inet_pton(AF_INET, ipServer, &serverAddress.sin_addr))
    {
        /* Receive an IP address from the user */
        if (0 == mem++)
          AddMessage(SYSTEMUSER, "Enter IP", 0);
        
        {
            printf("%s\n", SYSTEMACTION);
            PrintMessage(SYSTEMUSER, "Type 'x' to connect to localhost (127.0.0.1)", 1);
            strncpy(ipServer, ProcessMessage(64, 0), 64);
            
            if ('x' == ipServer[0])
                strncpy(ipServer, "127.0.0.1", 64);
        }
        
        AddMessage(CUSER, ipServer, 0);
        
        if(0 >= inet_pton(AF_INET, ipServer, &serverAddress.sin_addr))
        {
            PrintMessage(SYSTEMUSER, "Invalid address", 0);
        }
        printf("%s\n", SYSTEMACTION);
    }
    
    /* CONNECT: Establish a connection with the server.
     clientSocket - socket to be used in connection,
     serverAddress - address of the server,
     size - size of sockaddr_in structure */
    if (-1 == connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
    {
        AddMessage(SYSTEMUSER, "Connection failed", 0);
        close(clientSocket);
        return -1;
    }
    
    /* Send and receive user information */
    /*{
        char userInfo[20];
        char* tempColor = (char*)malloc(sizeof(char[1]));
        
        strncpy(userInfo, CUSER->userName, 20);
        sprintf(tempColor, "%d", (CUSER->userColor - 40));
        
        userInfo[17] = *tempColor;
        
        
        send(clientSocket, userInfo, MAXSIZE-1, MAXSIZE-1);
        
        if (0 > (num = recv(clientSocket, buffer, MAXSIZE, 0)))
        {
            perror("recv");
            return -1;
        }
        else if (0 == num)
        {
            AddMessage(SYSTEMUSER, "Connection closed", 0);
            
            return -1;
        }
        
        PrintMessage(CUSER, buffer, 0);
    }*/
    
    {
      AddMessage(SYSTEMUSER, "Connection established! Say hello;)", 0);
    }
    
    /* Allow to send/receive multiple times */
    while (1)
    {
        /* Send a message */
        if (-1 == SendMsg(clientSocket))
            break;

        /* Receive a message */
        if (-1 == ReceiveMsg(clientSocket))
            break;
            
        AddMessage(SYSTEMUSER, buffer, 0);
    }
    
    close(clientSocket);
    
    return 0;
}

int SendMsg(int xSocket)
{
    /* Input and save a message */
    strncpy(buffer, ProcessMessage(MAXSIZE, 1), MAXSIZE);
    /* Display it in the message history */
    AddMessage(CUSER, buffer, 0);
      
    /* SEND: Send data (blocking).
     buffer - message to send,
     SENDBUFFERSIZE - size of the message,
     0 - no flags */
    if (MAXSIZE != (send(xSocket, buffer, MAXSIZE, 0)))
    {
      AddMessage(SYSTEMUSER, "Failure Sending Message", 0);
        
      return -1;
    }
    
    return 0;
}

int ReceiveMsg(int xSocket)
{
    int msgSize = -1;
    USER* connectionUser = &(USER){ .userName = "connectionUser", .userColor = 46 };
  
    if (-1 == (msgSize = recv(xSocket, buffer, MAXSIZE, 0)))
    {
        AddMessage(SYSTEMUSER, "Connection Closed", 0);
        
        return -1;
    }
    
    /* Display a received message */
    AddMessage(SYSTEMUSER, buffer, 0);
    /**/
    
    return msgSize;
}
