/*
Author:  Cameron Allotey
Date last modified: 10/22/2019
Organization: ECE 4122 Class

Description:

C++ program to operate multithreaded tcp server
Based on class example
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <thread>         // std::thread
#include <vector>
#include <iomanip>
#include <algorithm>
#include "tcp.h"

/*Struct Header
struct used to track information about connected clients*/
struct client_info
{
    int portno;
    char* IP;
    int sockfd;
};

//type definitions for server management
typedef int SOCKET;
typedef std::vector<std::thread> thread_vec_t;  //vector of threads to manage active connections
typedef std::vector<struct client_info> client_vec_t; //vector of clients to manage connections

//function definitions
void handleInputs();
void handleClients(char** argv);
void listen_client(struct client_info info);

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
    // Make sure any open sockets are closed before calling exit
    exit(1);
}

//Global Variables 
thread_vec_t threads;
client_vec_t clients;
struct tcpMessage *lastMessage = NULL;

/////////////////////////////////////////////////
// Main
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    lastMessage = new struct tcpMessage();
    std::thread inputs(handleInputs);
    std::thread connections(handleClients, argv);
    inputs.join();
    connections.join();
    return 0;
}

/*Function Header
Function that allows the called thread to prompt user for input and act accordingly*/
void handleInputs()
{
    while (1) //constantly prompt until program close
    {
        printf("Please enter command: ");
        char input;
        std::cin>>input;
	std::cin.ignore();
        if (input == '0' && lastMessage != NULL) //print last message received
        {
            std::cout<<"Last Message: "<< lastMessage -> chMsg <<std::endl;
        } 
        else if (input == '1') //print client information
        {
            std::cout<<"Number of Clients: "<<clients.size()<<std::endl;
            if (clients.size() != 0)
            {
                std::cout<<std::setw(20)<< std::left <<"IP Address" << "Port"<<std::endl;
                for (int i = 0; i < clients.size(); ++i) 
                {
                    std::cout<<std::setw(20)<< std::left <<clients[i].IP<<clients[i].portno<<std::endl;
                }
            }
        }
        else if (input == 'q') //program close after closing all sockets
        {
            //clear last msg
            free(lastMessage);
            //close sockets
            for (int i = 0; i < clients.size(); i++)
            {
                sockClose(clients[i].sockfd);
            }
            exit(0);
        }
    }
}

/*Function Header
Function that allows the called thread to open server and listen for connections*/
void handleClients( char **argv)
{
    int sockfd, newsockfd, portno;

    socklen_t clilen;

    //socket structutre for reference
    /*struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
    };*/

    struct sockaddr_in serv_addr, cli_addr;
    
    int n;
    // Create the socket (domain, type, protocol)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Make sure the socket was created
    if (sockfd < 0)
        error("ERROR opening socket");

    // Zero out the variable serv_addr
    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    // Convert the port number string to an int
    portno = atoi(argv[1]);
    
    // Initialize the serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert port number from host to network
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    //listen for connections
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);
    while(1) //constantly listens for new connectons and creates new threads to handle new clients
    {
        // blocks until a client connects to the server
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
            error("ERROR on accept");

        //store client info
        struct client_info s_client;
        s_client.portno = ntohs(cli_addr.sin_port);
        s_client.IP = inet_ntoa(cli_addr.sin_addr);
        s_client.sockfd = newsockfd;
        clients.push_back(s_client);

        //create new thread to only listen for messages from this client
        std::thread client(listen_client, s_client);
        client.detach();
    }
}

/*Function Header
Function that allows the called thread to listen for messages from specific client*/
void listen_client(struct client_info info) 
{   
    //allocate memory for struct and buffer
    char serialized[sizeof(struct tcpMessage)];
    int n;
    memset(serialized, 0, sizeof(struct tcpMessage));

    while(1)
    {
        // blocks until the client connected on this socket sends a message
        n = recv(info.sockfd, serialized, sizeof(struct tcpMessage), 0);
        if (n < 0) //read error
            error("ERROR reading from socket");
        if (n == 0) //client disconnected
            break;
        serialized[n] = 0; // String was not null terminated
        deserialize(serialized, lastMessage);

        //temp copy of most recent messge in case it changes rapidly
        struct tcpMessage * temp = new struct tcpMessage();
        temp -> nVersion = lastMessage -> nVersion;
        temp -> nType = lastMessage -> nType;
        temp -> nMsgLen = lastMessage -> nMsgLen;
        for (int i = 0; i < strlen(lastMessage -> chMsg); i++)
        {
            temp -> chMsg[i] = lastMessage -> chMsg[i];
        }

        //contraints outlined in assignment
        if (lastMessage->nVersion != '1')
        {
            //do nothing
            continue;
        }
        if (lastMessage->nType == '0')
        {
            //send to all other clients
            memset(serialized, 0, sizeof(struct tcpMessage));
            serialize(temp, serialized);
            for (int i = 0; i < clients.size(); i++)
            {
                if (clients[i].sockfd != info.sockfd) {
                    n = send(clients[i].sockfd, serialized, 1004, 0);
                    if (n < 0)
                        error("ERROR writing to socket");
                }
            }
        }
        else if (lastMessage->nType == '1')
        {
            //reverse msg and send back
            std::reverse(temp->chMsg, temp->chMsg + temp->nMsgLen);
            memset(serialized, 0, sizeof(struct tcpMessage));
            serialize(temp, serialized);
            n = send(info.sockfd, serialized, 1004, 0);
            if (n < 0)
                error("ERROR writing to socket");
        }

    }
    //loop through to find appropriate client and remove
    for (int i = 0; i < clients.size(); i++) {
        if (clients[i].sockfd == info.sockfd) {
            clients.erase (clients.begin() + i);
        }
    }
}
