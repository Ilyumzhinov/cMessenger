#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for malloc(), exit() */
#include <unistd.h> /* for close() */
#include <string.h> /* for strcopy(), strlen(), memset() */
#include <arpa/inet.h> /* for struct sockaddr_in, SOCK_STREAM */

#define PORT 8080
#define MAXSIZE 1024
#define MAXPENDING 1

/*FUNCTIONS*/
/* Create a listening host using a socket */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/
 https://codereview.stackexchange.com/questions/13461/two-way-communication-in-tcp-server-client-implementation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */
int CreateServer(USER* connectionUser)
{
    int serverSocket; /* Socket descriptor for serverAddress */
    int clientSocket; /* Socket descriptor for client */
    int num;
    struct sockaddr_in serverAddress; /* Local address */
    struct sockaddr_in clientAddress; /* Client address */
    unsigned int size;
    
    const int cbufferSize = 32;
    int bufferSize;
    char buffer[MAXSIZE] = {0};
    
    /* Create socket for incoming connections */
    if (0 > (serverSocket = socket(AF_INET, SOCK_STREAM, 0)))
    {
        AddMessage(SYSTEMUSER, "Socket failure!", 0);
        exit(1);
    }
    
    /* Construct local address structure */
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_port = htons(PORT); /* Local port */
    serverAddress.sin_addr.s_addr = INADDR_ANY;  /* Any incoming interface */
    
    /* Bind to the local port */
    if (0 > (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr ))))
    {
        AddMessage(SYSTEMUSER, "Binding Failure!", 0);
        exit(1);
    }
    
    /* Mark the socket to listen for incoming connections */
    if (0 > listen(serverSocket, MAXPENDING))
    {
        AddMessage(SYSTEMUSER, "Listening Failure!", 0);
        exit(1);
    }
    
    for(;;)
    {
        size = sizeof(struct sockaddr_in);
        
        /* Wait for a client to connect */
        if (0 > (clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddress, &size)))
        {
            AddMessage(SYSTEMUSER, "Accept error!", 0);
            exit(1);
        }
        
        {
            AddMessage(SYSTEMUSER, "Server got connection from client", 0);
            
            /*printf("Server got connection from client %s\n", inet_ntoa(serverAddress.sin_addr));*/
            
            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0)))
            {
                AddMessage(SYSTEMUSER, "Recv error!", 0);
                break;
            }
            else if (0 == bufferSize)
            {
                AddMessage(SYSTEMUSER, "Connection closed", 0);
                
                break;
            }
            
            PrintMessage(CUSER, buffer, 0);
            
            connectionUser = (USER*)malloc(sizeof(USER));
            strncpy(connectionUser->userName, buffer, 16);
            
            connectionUser->userColor = atoi(buffer + 17);
            
            /* userName userColor */
            {
                char userInfo[20];
                char* tempColor = (char*)malloc(sizeof(char[1]));
                
                strncpy(userInfo, CUSER->userName, 20);
                /*sprintf(tempColor, "%d", (CUSER->userColor - 40));*/
                
                userInfo[17] = *tempColor;
                
                /*printf("server send: %s\n", userInfo);*/
                send(clientSocket, userInfo, MAXSIZE-1, MAXSIZE-1);
            }
        }
        
        for(;;)
        {
            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0)))
            {
                AddMessage(SYSTEMUSER, "recv error!", 0);
                exit(1);
            }
            else if (0 == bufferSize)
            {
                AddMessage(SYSTEMUSER, "Connection closed!", 0);
                break;
            }
            
            AddMessage(connectionUser, buffer, 0);
            
            /* Send a message */
            strncpy(buffer, ProcessMessage(MAXSIZE-1, 1), MAXSIZE-1);
            
            if (0 > (send(clientSocket, buffer, strlen(buffer), 0)))
            {
                AddMessage(SYSTEMUSER, "Send error!", 0);
                close(clientSocket);
                break;
            }
        }
        
        close(clientSocket);
    }
    
    close(serverSocket);
    return 0;
}

/* Create a client by connecting to a listening socket at a specified IP address */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/
 https://codereview.stackexchange.com/questions/13461/two-way-communication-in-tcp-server-client-implementation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */
int CreateClient()
{
    /* Socket data */
    int num;
    
    struct sockaddr_in address;
    int clientSocket = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[MAXSIZE] = {0};
    
    char ipServer[64];
    /**/
    
    if (0 > (clientSocket = socket(AF_INET, SOCK_STREAM, 0)))
    {
        AddMessage(SYSTEMUSER, "Socket creation error!", 0);
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    while (0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
    {
        /* Receive an IP address from the user */
        AddMessage(SYSTEMUSER, "Enter IP", 0);
        
        {
            printf("%s\n", SYSTEMACTION);
            PrintMessage(SYSTEMUSER, "Type 'x' to connect to localhost (127.0.0.1)", 1);
            strncpy(ipServer, ProcessMessage(64, 0), 64);
            
            if ('x' == ipServer[0])
                strncpy(ipServer, "127.0.0.1", 64);
        }
        
        AddMessage(CUSER, ipServer, 0);
        
        if(0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
        {
            printf("\nInvalid address or address not supported \n");
        }
    }
    
    if (0 > connect(clientSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        printf("\nPort is closed \n");
        return -1;
    }
    
    /* Send and receive user information */
    {
        /* userName userColor */
        char userInfo[20];
        char* tempColor = (char*)malloc(sizeof(char[1]));
        
        strncpy(userInfo, CUSER->userName, 20);
        sprintf(tempColor, "%d", (CUSER->userColor - 40));
        
        userInfo[17] = *tempColor;
        
        /*printf("client to send: %s\n",userInfo);*/
        
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
        
        /* connectionUser = (USER*)malloc(sizeof(USER));
        strncpy(connectionUser->userName, buffer, 16);
        
        connectionUser->userColor = atoi(buffer + 17);*/
    }
    
    for(;;)
    {
        /* Send a message */
        strncpy(buffer, ProcessMessage(MAXSIZE-1, 1), MAXSIZE-1);
        
        AddMessage(CUSER, buffer, 0);
        
        if (0 > (send(clientSocket,buffer, strlen(buffer),0)))
        {
            printf("Failure Sending Message\n");
            
            break;
        }
        else
        {
            num = recv(clientSocket, buffer, sizeof(buffer),0);
            
            if ( num <= 0 )
            {
                AddMessage(SYSTEMUSER, "Either Connection Closed or Error", 0);
                
                /* Break from the While */
                break;
            }
            
            /*AddMessage(connectionUser, buffer, 0);*/
        }
    }
    close(clientSocket);
    
    return 0;
}
