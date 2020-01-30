/*
Author:  Cameron Allotey
Date last modified: 10/21/2019
Organization: ECE 4122 Class

Description:

C++ header file for tcp communications
    
*/

/*Struct Header
struct used for messages between hosts*/
struct tcpMessage
{
    unsigned char nVersion;
    unsigned char nType;
    unsigned short nMsgLen;
    char chMsg[1000];
}; 

//function definitions
void serialize(struct tcpMessage* msg, char *data);
void deserialize(char *data, struct tcpMessage* msg);

/*Function Header
serializes the input struct and assigns the stream to the data pointer
Approach taken from Stack Overflow*/
void serialize(struct tcpMessage* msg, char *data)
{
    int *q = (int*)data;    
    *q = msg->nVersion;
    q++;    
    *q = msg->nType;
    q++;    
    *q = msg->nMsgLen;
    q++;
    
    char *p = (char*)q;
    int i = 0;
    while (i < 1000)
    {
        *p = msg->chMsg[i];
        p++;
        i++;
    }
}

/*Function Header
Populates struct based on the data pointer
Approach taken from Stack Overflow*/
void deserialize(char *data, struct tcpMessage* msg)
{
    int *q = (int*)data;    
    msg->nVersion = *q;
    q++;
    msg->nType = *q;
    q++;    
    msg->nMsgLen = *q;
    q++;

    char *p = (char*)q;
    int i = 0;
    while (i < 1000)
    {
        msg->chMsg[i] = *p;
        p++;
        i++;
    }
}
