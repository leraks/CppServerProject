#ifndef TCP_SERVER_SERVERFOLDER_SERVER_H
#define TCP_SERVER_SERVERFOLDER_SERVER_H

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>

#define PORT 5555

class Server {
 public:
  Server();
  ~Server();
  void SocketStart();
  void BindStart();
  void ListenStartAndFdSet(int l);
  void ServerWork();
  int ReadFromClient(int sockfd, char *buf);
  void WriteToClient(int sockfd, char *buf);
  void WriteAllUser(int sockfd);
  void WriteMessage(int sockfd, int userfd, const char *mail);
  void WriteCountUser(int sockfd);
  void WriteErrorMessage(int sockfd);
  std::vector<std::string> SplitMessage(const std::string &s, char delim);
  bool isValidMessage(const std::vector<std::string> &substrings);

 private:
  int sock_, new_sock_;
  int err_;
  fd_set active_set_, read_set_;
  int sock_open_;
  socklen_t size_;
  struct sockaddr_in addr_;
  struct sockaddr_in client_;
  char buf_[2048];
  std::set<int> clients;
};

#endif // TCP_SERVER_SERVERFOLDER_SERVER_H
