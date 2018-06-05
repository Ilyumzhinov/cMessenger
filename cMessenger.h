/* GLOBAL SYMBOLIC CONSTANTS */
#define USERNAMESIZE 16

/* STRUCTURES DEFINITIONS */
/* Structure that is used to store data about a user:
 Includes a username (16 characters max) and a color, saved as an int */
struct User
{
    char userName[USERNAMESIZE];
    
    /* Color Reference: https://misc.flogisoft.com/bash/tip_colors_and_formatting */
    int userColor;
} typedef USER;

/* Message structure is a linked-list node that is used to store data about a single message:
 Includes data of the sender (in the User structure's pointer),
 message string's pointer,
 indentation that tells the printing function whether the sender name should be printed
 and an address of the next message */
struct Message
{
    USER* sender;
    
    char* message;
    int indentation;
    
    struct Message* next;
} typedef MESSAGE;

/* Message History is a linked-list consisting of Message nodes. As such, it contains an address of the first Message structure */
struct MessageHistory
{
    MESSAGE* top;
} typedef MESSAGEHISTORY;
/**/

/* GLOBAL DECLARATION */
/* System user (cMessenger). Defined at cMessenger.c */
extern USER* SYSTEMUSER;
/* Current User. Defined at cMessenger.c
 The pointer cannot be changed, but the values pointed to can be */
extern USER* const CUSER;
/* Message thread of the session. Defined at cMessenger.c */
extern MESSAGEHISTORY* MESSAGEHIST;
/* Standard encapsulation of a system action. Defined at messageIO.c */
extern const char* SYSTEMACTION;
/**/

/* FUNCTIONS PROTOTYPES */
/* Create a user structure
 defined at dataStructures.c */
void CreateUser();

/* Add a new node to the dynamic structure
 defined at dataStructures.c */
void AddMessage(USER*, char*, int);

/* Create a listening host using a socket
 defined at connection.c */
int CreateServer();

/* Create a client by connecting to a listening socket at a specified IP address
 defined at connection.c */
int CreateClient();

/* A universal string processing method that includes systems calls.
 Allocates memory for a string with the size specified.
 By default, returns the pointer to the string.
 defined at printMessages.c */
char* ProcessMessage(int, int);

/* Print a message
 defined at printMessages.c */
void PrintMessage(USER*, char*, int);

/* Print a passed profile's information
 defined at printMessages.c */
void PrintProfile(USER*);

/* Traverse the dynamic strucre of messages
 defined at printMessages.c */
void PrintHistory(MESSAGE*);
