#ifndef CONNECTOR
#define CONNECTOR

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

void err(string s){
    perror(s.c_str());
    exit(1);
}

class connector
{
    int sockfd,port;
    struct sockaddr_in saddr;

public:
    
    connector(bool s_c, uint16_t port, string addr = "") // s_c is 1 for Server and 0 for Client
    {
        this->port=port;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (!~sockfd)
            err("Can't create socket.");
        memset(&saddr, 0, sizeof(sockaddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        if (s_c)
        {
            saddr.sin_addr.s_addr = htonl(INADDR_ANY);
            int reuse = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
                perror("setsockopt(SO_REUSEADDR) failed");
#ifdef SO_REUSEPORT
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse, sizeof(reuse)) < 0)
                perror("setsockopt(SO_REUSEPORT) failed");
#endif
            if (!~bind(sockfd, (sockaddr *)&saddr, sizeof saddr))
                err("Can't bind the socket.");
        }
        else if (!~inet_pton(AF_INET, addr.c_str(), &saddr.sin_addr))
            err("Wrong address format.");
    }
    // Client side functions:
    int connectToServer()
    {
        if (!~connect(sockfd, (sockaddr *)&saddr, sizeof saddr))
            err("Cannot connect.");
        return sockfd;
    }
    // Server side functions:
    void listenForClients()
    {
        if (!~listen(sockfd, 10))
            err("Cannot listen.");
        cout << "Waiting for Connections on port: " << port << endl;
    }
    int acceptNow()
    {
        int new_fd;
        socklen_t a = sizeof(saddr);
        if (!~(new_fd = accept(sockfd, (sockaddr *)&saddr, (socklen_t *)&a)))
            err("Cannot accept.");
        return new_fd;
    }
    void closeServer()
    {
        close(sockfd);
    }
};

#endif