/* GLOBAL SYMBOLIC CONSTANTS */
/* Standard encapsulation of a system action */
#define SYSTEMACTION "--------------------"
/**/


/* STRUCTURES DEFINITIONS FOR cMESSENGER */
/* Structure that is used to store data about a user:
 Includes a username (16 characters max) and a color, saved as an int */
struct User
{
    char userName[16];
    
    /* Reference: https://misc.flogisoft.com/bash/tip_colors_and_formatting */
    int userColor;
};

/* Message structure is a linked-list node that is used to store data about a single message:
 Includes data of the sender (in the User structure),
 message string,
 indentation that tells the printing function whether the sender name should be printed
 and an address of the next message */
struct Message
{
    struct User* sender;
    
    char* message;
    int indentation;
    
    struct Message* next;
};

/* Message History is a linked-list consisting of Message nodes. As such, it contains an address of the first Message structure */
struct MessageHistory
{
    struct Message* top;
};
/**/


/* FUNCTIONS PROTOTYPES FOR cMESSENGER */
/* Create a user structure */
struct User* CreateUser(struct User*, struct User*, struct MessageHistory*);

/* Create a listening host using a socket */
int CreateServer(struct User*, struct User*, struct User*, struct MessageHistory*);

/* Create a client by connecting to a listening socket at a specified IP address */
int CreateClient(struct User*, struct User*, struct MessageHistory*);

/* FUNCTIONS PROTOTYPES FOR cMESSENGER */
/* A universal string processing method that includes systems calls.
 Allocates memory for a string with the size specified.
 By default, returns the pointer to the string.
 */
char* ProcessMessage(int, int, struct MessageHistory*);

/* Print a message */
void PrintMessage(struct User*, char*, int);

/* Add a new node to the dynamic structure */
void AddMessage(struct MessageHistory*, struct User*, char*, int);

/* Traverse the dynamic strucre of messages */
void PrintHistory(struct Message*);
