#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for malloc() */
#include <string.h> /* for strcopy() */

/*FUNCTIONS*/
/* A complete procedure that lets a user to create a USER structure and returns it */
void CreateUser()
{
    char userColorInput[1];
    
    AddMessage(SYSTEMUSER, "To create/modify a user profile", 1);
    
    /* Create name */
    {
        AddMessage(SYSTEMUSER, "Type in your nickname", 1);
        
        strncpy(CUSER->userName, ProcessMessage(USERNAMESIZE, 0), USERNAMESIZE);
        
        AddMessage(CUSER, CUSER->userName, 0);
    }
    
    /* Choose color */
    {
        int i;
        char* tempColor = (char*)malloc(sizeof(char[64]));
        
        /* Print various colors */
        AddMessage(SYSTEMUSER, "Choose color", 0);
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
        
        CUSER->userColor = atoi(userColorInput) + 40;
        
        /* Reference: http://forums.codeguru.com/showthread.php?347081-itoa-error */
        sprintf(tempColor, "%d", CUSER->userColor - 40);
        
        AddMessage(CUSER, tempColor, 0);
    }
}

/* Add a new node to the dynamic structure */
void AddMessage(USER* userPtr, char* messageStr, int indentation)
{
    struct Message* iMessage;
    struct Message* tempMessage;
    
    iMessage = MESSAGEHIST->top;
    
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
        MESSAGEHIST->top = tempMessage;
    }
    else
    {
        while (NULL != iMessage->next)
        {
            iMessage = iMessage->next;
        }
        
        iMessage->next = tempMessage;
    }
    
    PrintHistory(MESSAGEHIST->top);
}
