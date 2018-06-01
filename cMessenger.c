/* Compile:
    gcc cMessenger.c printMessages.c establishConnection.c -o cMessenger
*/
#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for malloc() */
#include <string.h> /* for strcopy() */

/* Main logic */
int main()
{
    /* Declaration */
    #define MENUCHOICESIZE 1
    
    /* Reference: https://stackoverflow.com/questions/11709929/how-to-initialize-a-pointer-to-a-struct-in-c */
    struct User* systemUser = &(struct User){ .userName = "cMessenger", .userColor = 100 };
    struct User* currentUser = NULL;
    struct User* connectionUser = NULL;
    struct MessageHistory* messageHistory = NULL;
    
	char menuChoice[MENUCHOICESIZE];
    char* strPtr;

    messageHistory = (struct MessageHistory*)malloc(sizeof(struct MessageHistory));
	messageHistory->top = NULL;
	/**/

	for(;;)
	{
        menuChoice[0] = '\0';

	    if (NULL == currentUser)
	    {
	        /* Welcome message */
	       	AddMessage(messageHistory, systemUser, "Welcome!", 0);

	        /* Creating a user */
	        CreateUser(systemUser, currentUser, messageHistory);
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
	        strncpy(menuChoice, ProcessMessage(1, 0, messageHistory), 1);
	    }

	    AddMessage(messageHistory, currentUser, menuChoice, 0);

		/* Open chat */
		if (49 == menuChoice[0])
		{
			CreateServer(systemUser, currentUser, connectionUser, messageHistory);
		}
	    /* Join chat */
	    else if (50 == menuChoice[0])
	    {
	        CreateClient(systemUser, currentUser, messageHistory);
	    }
	}
}
