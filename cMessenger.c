/* Compile:
    gcc cMessenger.c messageIO.c dataStructures.c connectivity.c -o cMessenger
*/
#include "cMessenger.h"

#include <stdio.h> /* for printf() */
#include <string.h> /* for strcopy() */

/* GLOBAL DEFINITION */
/* Initialise a global system user constant */
USER* SYSTEMUSER = &(USER){ .userName = "cMessenger", .userColor = 100 };
/* Initialise a global current user pointer constant */
USER* const CUSER = &(USER){ .userName = "\0", .userColor = 0 };
/* Initialise a constant linked-list structure that keeps track of messages */
MESSAGEHISTORY* MESSAGEHIST = &(MESSAGEHISTORY){ .top = NULL };
/**/

/* Main logic */
int main()
{
    /* Declaration */
    #define MENUCHOICESIZE 1
    
    /* Current user information (set up at the beginning of the program execution) */
    /* Connection user information is received when a user connect to a server */
    USER* connectionUser = NULL;
    /* Menu option selected */
	char menuChoice[MENUCHOICESIZE];
	/**/

    /* Create a user */
    {
        /* Welcome message */
        AddMessage(SYSTEMUSER, "Welcome!", 0);
        
        /* Create a user */
        CreateUser();
    }
    
    /* Continue with the program execution until the system character is typed in */
	for(;;)
	{
        menuChoice[0] = '\0';

	    /* Print main menu */
	    {
	    	AddMessage(SYSTEMUSER, "Choose from the options", 0);
	    	{
	    		printf("%s\n", SYSTEMACTION);
		    	PrintMessage(SYSTEMUSER, "1 | Open chat", 1);
		    	PrintMessage(SYSTEMUSER, "2 | Join chat", 1);
		    	printf("%s\n", SYSTEMACTION);
		    }
	    }

	    /* Processing the menu choice */
	    while (menuChoice[0] < 49 || menuChoice[0] > 50)
	    {
	        strncpy(menuChoice, ProcessMessage(1, 0), 1);
	    }

	    AddMessage(CUSER, menuChoice, 0);

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
