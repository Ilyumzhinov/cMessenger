#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for exit() */
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
int CreateServer(struct User* sysUser, struct User* currentUser, struct User* connectionUser, struct MessageHistory* messageHistory)
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
        AddMessage(messageHistory, sysUser, "Socket failure!", 0);
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
        AddMessage(messageHistory, sysUser, "Binding Failure!", 0);
        exit(1);
    }
    
    /* Mark the socket to listen for incoming connections */
    if (0 > listen(serverSocket, MAXPENDING))
    {
        AddMessage(messageHistory, sysUser, "Listening Failure!", 0);
        exit(1);
    }
    
    for(;;)
    {
        size = sizeof(struct sockaddr_in);
        
        /* Wait for a client to connect */
        if (0 > (clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddress, &size)))
        {
            AddMessage(messageHistory, sysUser, "Accept error!", 0);
            exit(1);
        }
        
        {
            AddMessage(messageHistory, sysUser, "Server got connection from client", 0);
            
            /*printf("Server got connection from client %s\n", inet_ntoa(serverAddress.sin_addr));*/
            
            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0)))
            {
                AddMessage(messageHistory, sysUser, "Recv error!", 0);
                break;
            }
            else if (0 == bufferSize)
            {
                AddMessage(messageHistory, sysUser, "Connection closed", 0);
                
                break;
            }
            
            PrintMessage(currentUser, buffer, 0);
            
            connectionUser = (struct User*)malloc(sizeof(struct User));
            strncpy(connectionUser->userName, buffer, 16);
            
            connectionUser->userColor = atoi(buffer + 17);
            
            /* userName userColor */
            {
                char userInfo[20];
                char* tempColor = (char*)malloc(sizeof(char[1]));
                
                strncpy(userInfo, currentUser->userName, 20);
                /*sprintf(tempColor, "%d", (currentUser->userColor - 40));*/
                
                userInfo[17] = *tempColor;
                
                /*printf("server send: %s\n", userInfo);*/
                send(clientSocket, userInfo, MAXSIZE-1, MAXSIZE-1);
            }
        }
        
        for(;;)
        {
            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0)))
            {
                AddMessage(messageHistory, sysUser, "recv error!", 0);
                exit(1);
            }
            else if (0 == bufferSize)
            {
                AddMessage(messageHistory, sysUser, "Connection closed!", 0);
                break;
            }
            
            AddMessage(messageHistory, connectionUser, buffer, 0);
            
            /* Send a message */
            strncpy(buffer, ProcessMessage(MAXSIZE-1, 1, messageHistory), MAXSIZE-1);
            
            if (0 > (send(clientSocket, buffer, strlen(buffer), 0)))
            {
                AddMessage(messageHistory, sysUser, "Send error!", 0);
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
int CreateClient(struct User* sysUser, struct User* currentUser, struct MessageHistory* messageHistory)
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
        AddMessage(messageHistory, sysUser, "Socket creation error!", 0);
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    while (0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
    {
        /* Receive an IP address from the user */
        AddMessage(messageHistory, sysUser, "Enter IP", 0);
        
        {
            
            printf("%s\n", SYSTEMACTION);
            PrintMessage(sysUser, "Type 'x' to connect to localhost (127.0.0.1)", 1);
            strncpy(ipServer, ProcessMessage(64, 0, messageHistory), 64);
            
            if ('x' == ipServer[0])
                strncpy(ipServer, "127.0.0.1", 64);
        }
        
        AddMessage(messageHistory, currentUser, ipServer, 0);
        
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
        
        strncpy(userInfo, currentUser->userName, 20);
        sprintf(tempColor, "%d", (currentUser->userColor - 40));
        
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
            AddMessage(messageHistory, sysUser, "Connection closed", 0);
            
            return -1;
        }
        
        PrintMessage(currentUser, buffer, 0);
        
        /* connectionUser = (struct User*)malloc(sizeof(struct User));
        strncpy(connectionUser->userName, buffer, 16);
        
        connectionUser->userColor = atoi(buffer + 17);*/
    }
    
    for(;;)
    {
        /* Send a message */
        strncpy(buffer, ProcessMessage(MAXSIZE-1, 1, messageHistory), MAXSIZE-1);
        
        AddMessage(messageHistory, currentUser, buffer, 0);
        
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
                AddMessage(messageHistory, sysUser, "Either Connection Closed or Error", 0);
                
                /* Break from the While */
                break;
            }
            
            /*AddMessage(messageHistory, connectionUser, buffer, 0);*/
        }
    }
    close(clientSocket);
    
    return 0;
}

struct User* CreateUser(struct User* sysUser, struct User* currentUser, struct MessageHistory* messageHistory)
{
    struct User* userPtr = (struct User*)malloc(sizeof(struct User));
    char userColorInput[1];
    
    /* Choose name */
    {
        AddMessage(messageHistory, sysUser, "Type in your nickname", 1);
        
        strncpy(userPtr->userName, ProcessMessage(16, 0, messageHistory), 16);
        
        currentUser = userPtr;
        
        AddMessage(messageHistory, userPtr, userPtr->userName, 0);
    }
    
    /* Choose color */
    {
        int i;
        char* tempColor = (char*)malloc(sizeof(char[64]));
        
        /* Print various colors */
        AddMessage(messageHistory, sysUser, "Choose color", 0);
        
        /* Print system action */
        {
            printf("%s\n", SYSTEMACTION);
            for (i = 1; i < 6; i++)
            {
                printf("\x1b[97;%dm %d - %s \x1b[0m ", i + 40, i, "message");
            }
            printf("\n%s\n", SYSTEMACTION);
        }
        
        while (userColorInput[0] < 49 || userColorInput[0] > 53)
        {
            strncpy(userColorInput, ProcessMessage(1, 0, messageHistory), 1);
        }
        
        userPtr->userColor = atoi(userColorInput) + 40;
        
        /* Reference: http://forums.codeguru.com/showthread.php?347081-itoa-error */
        sprintf(tempColor, "%d", userPtr->userColor - 40);
        
        AddMessage(messageHistory, userPtr, tempColor, 0);
    }
    
    return userPtr;
}
