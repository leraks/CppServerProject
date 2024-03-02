#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#define SERVER_NAME "127.0.0.1"
#define SERVER_PORT 5555

class Client{
    public:
        Client();
        ~Client();
        void SocketStart();
        void Connect();
        static void readInput(std::string& data);
        static void ReceiveMessages(int sockfd);
        void ClientWork();
        int ReadFromServer(int sockfd);
        int WriteToServer(int sockfd);
    private:
        int sock_;
        int connect_err_;
        int sock_open_;
        fd_set active_set_, read_set_;
        struct sockaddr_in server_addr_;
        struct hostent* hostinfo_;
    
};