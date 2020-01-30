/*
Author:  Cameron Allotey
Date last modified: 10/22/2019
Organization: ECE 4122 Class

Description:

C++ program to operate multithreaded tcp client
Based on class example
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <iostream>

/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <string>
#include <thread>
#include "tcp.h"

//type definitions for connection function
typedef int SOCKET;

//function definitions
void handleConnection(int sockfd);

/////////////////////////////////////////////////
// linux socket close
int sockClose(SOCKET sock)
{

    int status = 0;

    status = shutdown(sock, SHUT_RDWR);
    if (status == 0)
    {
        status = close(sock);
    }
    return status;

}

/////////////////////////////////////////////////
// Output error message and exit
void error(const char *msg)
{
    perror(msg);

    exit(0);
}

//Global Variables 
unsigned char version = 0;
struct tcpMessage *s_msg = NULL;

/////////////////////////////////////////////////
// Main
int main(int argc, char *argv[])
{
    s_msg = (struct tcpMessage*)calloc(1, sizeof(struct tcpMessage));
    //put main socket setup here
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }


    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1000];

    // Convert string to int
    portno = atoi(argv[2]);
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    // Zero out serv_addr variable
    memset((char *)&serv_addr, sizeof(serv_addr), 0);
    
    serv_addr.sin_family = AF_INET;

    memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");


    std::thread connections(handleConnection, sockfd);
    while (1) 
    {
        std::cout<<"Please enter command: ";
        char flag;
        unsigned char num;
        unsigned char type = 0;
        std::string input;
        std::getline(std::cin, input);
        flag = input[0];
        if (input.size() > 1 && input[1] != ' ')
        {
            continue;
        }
        if (flag == 'v')
        {
            version = input[2];
        } 
        else if (flag == 't')
        {
            type = input[2];
            if (input[3] != ' ')
            {
                continue;
            }
            input.erase(0,4);
            if (input.length() > 1000)
            {
                input = input.substr(0, 1000);
            }

            //make struct and send msg
            s_msg->nVersion = version;
            s_msg->nType = type;
            s_msg->nMsgLen = input.length();
            strcpy(s_msg->chMsg, input.c_str());

            char serialized[sizeof(struct tcpMessage)];
            serialize(s_msg, serialized);
            n = send(sockfd, serialized, sizeof(struct tcpMessage), 0);

            if (n < 0)
                error("ERROR writing to socket");
        }
        else if (flag == 'q')
        {
            //close sockets
            sockClose(sockfd);
            break;
        }
    }
    connections.join();
    free(s_msg);
    return 0;
}

/*Function Header
Function that allows the called thread to listen for messages from the given socket*/
void handleConnection( int sockfd)
{
    char buffer[1004];
    int n = 0;
    memset(buffer, 0, 1004);
    while (1)
    {
        n = recv(sockfd, buffer, 1004, 0);
        if (n < 0)
            error("ERROR reading from socket");
        if (n != 0)
        {
            buffer[n] = 0;
            struct tcpMessage * temp = new struct tcpMessage();
            deserialize(buffer, temp);
            std::cout<<std::flush;
            std::cout<<"\nReceived Msg Type: "<<temp->nType<<"; Msg: "<<temp->chMsg<<std::endl;
            std::cout<<"Please enter command: "<<std::flush;
        }
        else
        {
            break;
        }
    }
}