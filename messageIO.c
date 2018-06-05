#include "cMessenger.h"

#include <stdio.h> /* for printf(), fgets(), scanf() */
#include <stdlib.h> /* for malloc(), free(), exit() */
#include <string.h> /* for strlen() */

#define LALIGN 20
#define RALIGN 80
#define MSGLINE "message >"

/* GLOBAL DEFINITION */
/* Initialise a system action separator */
const char* SYSTEMACTION = "--------------------";
/**/

/*FUNCTIONS*/
/* A universal string processing method that includes systems calls.
 Allocates memory for a string with the size specified (then scaled to system calls requirement).
 By default, returns the pointer to the string. */
char* ProcessMessage(int size, int isInChat)
{
    /* Declaration */
    /* Scale the size, so even it is small, the program will be able to process a system call */
    #define MINSYSCALLSIZE 2
    
    char* messageStr;
    int i;
    /**/
    
    /* Scale the size if necessary */
    if (size < MINSYSCALLSIZE)
        size = MINSYSCALLSIZE;
    
    messageStr = (char*)malloc(size* sizeof(char));
    
    /* Print standard system action UI */
    {
	    printf("%s\n", SYSTEMACTION);
	    printf(MSGLINE);

        /* Insert additional spaces after the nickname */
	    for (i = strlen(MSGLINE); i < LALIGN; i++)
	        printf(" ");

        /* Save the input on a character basis */
        {
            int j = 0;
            char temp = 0;
            
            while ((temp = getchar()) && temp != '\n' && temp != EOF)
            {
                if (j < size)
                {
                    messageStr[j] = temp;
                    j++;
                }
            }
        }
        
	    printf("%s\n", SYSTEMACTION);
	}

    /* Process the string */
    /* If it starts with "/", it contains a system call */
    if (47 == messageStr[0])
    {
        /* If "/c", close the program */
        if (99 == messageStr[1])
        {
        	AddMessage(SYSTEMUSER, "Goodbye!", 0);

            free(messageStr);

            exit(0);
        }
        /* If "/h", print help commands */
        else if (104 == messageStr[1])
        {
        	printf("%s\n", SYSTEMACTION);
	    	PrintMessage(SYSTEMUSER, "List of system commands", 0);
	    	PrintMessage(SYSTEMUSER, "/c | Close the program", 1);
	    	PrintMessage(SYSTEMUSER, "/h | Help", 1);
            PrintMessage(SYSTEMUSER, "/m | Modify profile", 1);
            PrintMessage(SYSTEMUSER, "/p | Profile info", 1);
	    	printf("%s\n", SYSTEMACTION);
        }
        /* If "/m", modify profile */
        else if (109 == messageStr[1])
        {
            CreateUser();
        }
        /* If "/p", print profile info */
        else if (112 == messageStr[1])
        {
            printf("%s\n", SYSTEMACTION);
            PrintProfile(CUSER);
            printf("%s\n", SYSTEMACTION);
        }

        /* System call processed, now release memory */
        free(messageStr);
        /* Repeat until user's input is a normal message */
        messageStr = ProcessMessage(size, isInChat);
    }

    /*if (1 == isInChat)
    {
        fgets(messageStr, size, stdin);
    }*/

    /* Return the string */
    return messageStr;
}

/* Print a formatted message:
 Align according to LALIGN and RALIGN.
 Print with or without a username. */
void PrintMessage(USER* user, char* messageStr, int woName)
{
    int isRight = 0, indentation = 1;

    /* If user is current user, make right-indentation */
    if (user == CUSER)
    {
        if (!strncmp(user->userName, CUSER->userName, 16))
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

/* Print a passed profile's information */
void PrintProfile(USER* profilePrt)
{
    printf("Profile's name: %s (%p)\n", profilePrt->userName, profilePrt->userName);
    printf("Profile's color: %d (%p)\n", profilePrt->userColor, &profilePrt->userColor);
}

/* Traverse the dynamic structure of messages */
void PrintHistory(MESSAGE* currentMessage)
{
    struct Message* iMessage;

    /* "Clears" the terminal window */
    system("clear");

    iMessage = currentMessage;
    while (NULL != iMessage)
    {
        PrintMessage(iMessage->sender, iMessage->message, iMessage->indentation);
        iMessage = iMessage->next;
    }
}
