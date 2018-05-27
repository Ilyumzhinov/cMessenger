/* Compile:
    gcc cMessenger.c -o cMessenger
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXSIZE 1024
#define MAXPENDING 1

#define SYSTEMACTION "--------------------"
#define LALIGN 20
#define RALIGN 80

/*GLOBAL DECLARATION*/
/* Structures */
struct User
{
    char userName[16];

    /* Reference: https://misc.flogisoft.com/bash/tip_colors_and_formatting */
    int userColor;
};

struct Message
{
    struct User* sender;

    char* message;
    int indentation;

    struct Message* next;
};

struct MessageHistory
{
    struct Message* top;
};

/* Functions prototypes */
struct User* CreateUser();
int CreateServer();
int CreateClient();

char* ProcessMessage(int, int);
void AddMessage(struct MessageHistory*, struct User*, char*, int);
void PrintMessage(struct User*, char*, int);
void PrintHistory(struct Message*);

/* Global variables */
/* Reference: https://stackoverflow.com/questions/11709929/how-to-initialize-a-pointer-to-a-struct-in-c */
struct User* systemUser = &(struct User){ .userName = "cMessenger", .userColor = 100 };
struct User* currentUser = NULL;
struct User* connectionUser = NULL;

struct MessageHistory* messageHistory = NULL;
/**/


/*FUNCTIONS*/
/* Create a listening host using a socket */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ 
    https://codereview.stackexchange.com/questions/13461/two-way-communication-in-tcp-server-client-implementation?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */
int CreateServer()
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
        perror("Socket failure!!\n");
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
        perror("Binding Failure\n");
        exit(1);
    }

    /* Mark the socket to listen for incoming connections */
    if (0 > listen(serverSocket, MAXPENDING))
    {
        perror("Listening Failure\n");
        exit(1);
    }

    for(;;) 
    {
        size = sizeof(struct sockaddr_in);

        /* Wait for a client to connect */
        if (0 > (clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddress, &size))) 
        {
            perror("accept");
            exit(1);
        }

        {
            AddMessage(messageHistory, systemUser, "Server got connection from client", 0);

            /*printf("Server got connection from client %s\n", inet_ntoa(serverAddress.sin_addr));*/

            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0))) 
            {
                perror("recv");
                break;
            }
            else if (0 == bufferSize) 
            {
                AddMessage(messageHistory, systemUser, "Connection closed", 0);

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
                sprintf(tempColor, "%d", (currentUser->userColor - 40));

                userInfo[17] = *tempColor;

                /*printf("server send: %s\n", userInfo);*/
                send(clientSocket, userInfo, MAXSIZE-1, MAXSIZE-1);
            }
        }

        for(;;)
        {
            if (0 > (bufferSize = recv(clientSocket, buffer, cbufferSize, 0))) 
            {
                perror("recv");
                exit(1);
            }
            else if (0 == bufferSize) 
            {
                AddMessage(messageHistory, systemUser, "Connection closed", 0);

                break;
            }

            AddMessage(messageHistory, connectionUser, buffer, 0);
            
            /* Send a message */
            strncpy(buffer, ProcessMessage(MAXSIZE-1, 1), MAXSIZE-1);

            if (0 > (send(clientSocket, buffer, strlen(buffer), 0))) 
            {
                perror("send");
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
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    while (0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
    {
    	/* Receive an IP address from the user */
    	AddMessage(messageHistory, systemUser, "Enter IP", 0);
    	
    	{
	    	printf("%s\n", SYSTEMACTION);
		    PrintMessage(systemUser, "Type 'x' to connect to localhost (127.0.0.1)", 1);
	        strncpy(ipServer, ProcessMessage(64, 0), 64);

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
            AddMessage(messageHistory, systemUser, "Connection closed", 0);

            return -1;
        }

        PrintMessage(currentUser, buffer, 0);

        connectionUser = (struct User*)malloc(sizeof(struct User));
        strncpy(connectionUser->userName, buffer, 16);

        connectionUser->userColor = atoi(buffer + 17);
    }

    for(;;)
     {     
        /* Send a message */
        strncpy(buffer, ProcessMessage(MAXSIZE-1, 1), MAXSIZE-1);

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
                AddMessage(messageHistory, systemUser, "Either Connection Closed or Error", 0);

                /* Break from the While */
                break;
            }

            AddMessage(messageHistory, connectionUser, buffer, 0);
       }
    }
    close(clientSocket);

    return 0;
}

struct User* CreateUser()
{
    struct User* userPtr = (struct User*)malloc(sizeof(struct User));
    char userColorInput[1];

    /* Choose name */
    {
    	AddMessage(messageHistory, systemUser, "Type in your nickname", 1);

        strncpy(userPtr->userName, ProcessMessage(16, 0), 16);

        currentUser = userPtr;

        AddMessage(messageHistory, userPtr, userPtr->userName, 0);
    }

    /* Choose color */
    {
        int i;
        char* tempColor = (char*)malloc(sizeof(char[64]));

        /* Print various colors */
        AddMessage(messageHistory, systemUser, "Choose color", 0);

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
            strncpy(userColorInput, ProcessMessage(1, 0), 1);
        }

        userPtr->userColor = atoi(userColorInput) + 40;

        /* Reference: http://forums.codeguru.com/showthread.php?347081-itoa-error */
        sprintf(tempColor, "%d", userPtr->userColor - 40);

        AddMessage(messageHistory, userPtr, tempColor, 0);
    }

    return userPtr;
}

/* A universal string processing method that includes systems calls.
    Allocates memory for a string with the size specified.
    By default, returns the pointer to the string.
    */
char* ProcessMessage(int size, int isInChat)
{
    /* Declaration */
    char* messageStr = (char*)malloc(sizeof(char[size]));
    int i;
    /**/

    /* Print standard system action UI */
    {
	    printf("%s\n", SYSTEMACTION);
	    printf("message >");

	    for (i = strlen("message >"); i < LALIGN; i++)
	        printf(" ");

	    scanf("%s", messageStr);
	    printf("%s\n", SYSTEMACTION);
	}

    /* Process the string */
    /* If it starts with "/", it contains a system call */
    if (47 == messageStr[0])
    {
        /* If "/c", close the program */
        if (99 == messageStr[1])
        {
        	AddMessage(messageHistory, systemUser, "Goodbye!", 0);

            free(messageStr);

            exit(0);
        }
        /* If "/h", print help commands */
        else if (104 == messageStr[1])
        {
        	printf("%s\n", SYSTEMACTION);
	    	PrintMessage(systemUser, "List of system commands", 0);
	    	PrintMessage(systemUser, "/c | Close the program", 1);
	    	PrintMessage(systemUser, "/h | Help", 1);
	    	printf("%s\n", SYSTEMACTION);
        }

        free(messageStr);
        messageStr = ProcessMessage(size, 0);
    }

    if (1 == isInChat)
    {
        fgets(messageStr, size, stdin);
    }

    /* Return the string */
    return messageStr;
}

/* Print a message */
void PrintMessage(struct User* user, char* messageStr, int woName)
{
    int isRight = 0, indentation = 1;

    if (NULL != currentUser)
    {
        if (!strncmp(user->userName, currentUser->userName, 16))
        {
            isRight = 1;
        }
    }

    /* Print user name */
    if (0 == woName)
    {
        printf("%s: ", user->userName);
        indentation = strlen(user->userName) + 3;
    }

    /* Fill out the space before left alignment */
    while (indentation < LALIGN)
    {
        printf(" ");
        indentation++;
    }

    /* If the message is right-aligned, align according to right alignment */
    if (isRight)
    {
        while (indentation < (RALIGN - strlen(messageStr) - strlen(user->userName)))
        {
            printf(" ");
            indentation++;
        }
    }

    /* Print the message */
    printf("\x1b[97;%dm %s \x1b[0m\n", user->userColor, messageStr);
}

/* Add a new node to the dynamic structure */
void AddMessage(struct MessageHistory* historyPtr, struct User* userPtr, char* messageStr, int indentation)
{
    struct Message* iMessage;
    struct Message* tempMessage;

    iMessage = historyPtr->top;
    
    /* Fill out the new message */
    {
        tempMessage = (struct Message*)malloc(sizeof(struct Message));
        tempMessage->sender = userPtr;

        tempMessage->message = messageStr;
        tempMessage->indentation = indentation;

        tempMessage->next = NULL;
    }

    if (NULL == iMessage)
    {
        historyPtr->top = tempMessage;
    }
    else
    {
        while (NULL != iMessage->next)
        {
            iMessage = iMessage->next;
        }

        iMessage->next = tempMessage;
    }

    PrintHistory(messageHistory->top);
}

/* Traverse the dynamic strucre of messages */
void PrintHistory(struct Message* currentMessage)
{
    struct Message* iMessage;

    system("clear");

    iMessage = currentMessage;
    while (NULL != iMessage)
    {
        PrintMessage(iMessage->sender, iMessage->message, iMessage->indentation);
        iMessage = iMessage->next;
    }
}


/* Main logic */
int main()
{
	/* Declaration */
	char menuChoice[1];
    char* strPtr;

    messageHistory = (struct MessageHistory*)malloc(sizeof(struct MessageHistory));
	messageHistory->top = NULL;
	/**/

	for(;;)
	{
		strncpy(menuChoice, "\0", 1);

	    if (NULL == currentUser)
	    {
	        /* Welcome message */
	       	AddMessage(messageHistory, systemUser, "Welcome!", 0);

	        /* Creating a user */
	        CreateUser();
	        /**/
	    }

	    /* Printing main menu */
	    {
	    	AddMessage(messageHistory, systemUser, "Choose from the options", 0);

	    	{
	    		printf("%s\n", SYSTEMACTION);
		    	PrintMessage(systemUser, "1 | Open chat", 1);
		    	PrintMessage(systemUser, "2 | Join chat", 1);
		    	printf("%s\n", SYSTEMACTION);
		    }
	    }

	    /* Processing the menu choice */
	    while (menuChoice[0] < 49 || menuChoice[0] > 50)
	    {
	        strncpy(menuChoice, ProcessMessage(1, 0), 1);
	    }

	    AddMessage(messageHistory, currentUser, menuChoice, 0);

		/* Open chat */
		if (49 == menuChoice[0])
		{
			CreateServer();
		}
	    /* Join chat */
	    else if (50 == menuChoice[0])
	    {
	        CreateClient();
	    }
	}
}