#include "cMessenger.h"

#include <stdio.h> /* for printf(), fgets(), scanf() */
#include <stdlib.h> /* for malloc(), free(), exit() */
#include <string.h> /* for strlen() */

#define LALIGN 20
#define RALIGN 80

struct User* systemUser = &(struct User){ .userName = "cMessenger", .userColor = 100 };

/*FUNCTIONS*/
/* A universal string processing method that includes systems calls.
    Allocates memory for a string with the size specified.
    By default, returns the pointer to the string.
    */
char* ProcessMessage(int size, int isInChat, struct MessageHistory* messageHistory)
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

        /* System call processed, now release memory */
        free(messageStr);
        /* Repeat until user's input is a normal message */
        messageStr = ProcessMessage(size, 0, messageHistory);
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

    /* If user is current user, make right-indentation */
    /*if (NULL != currentUser)
    {
        if (!strncmp(user->userName, currentUser->userName, 16))
        {
            isRight = 1;
        }
    }*/

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

    PrintHistory(historyPtr->top);
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
