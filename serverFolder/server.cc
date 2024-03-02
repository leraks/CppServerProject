#include "server.h"

Server::Server() {
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(PORT);
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

Server::~Server() {
  if (sock_open_ == 1) {
    close(sock_);
  }
}

void Server::SocketStart() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    std::cout << "Error";
    exit(EXIT_FAILURE);
  }
  fcntl(sock_, F_SETFL, O_NONBLOCK);
  sock_open_ = 1;
}

void Server::BindStart() {
  err_ = bind(sock_, (struct sockaddr *)&addr_, sizeof(addr_));
  if (err_ < 0) {
    std::cout << "Error bind" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Server::ListenStartAndFdSet(int l) {
  err_ = listen(sock_, l);
  if (err_ < 0) {
    std::cout << "Error listen" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Server::ServerWork() {
  clients.clear();

  while (1) {
    FD_ZERO(&read_set_);
    FD_SET(sock_, &read_set_);

    for (std::set<int>::iterator it = clients.begin(); it != clients.end();
         it++)
      FD_SET(*it, &read_set_);

    int mx = std::max(sock_, *max_element(clients.begin(), clients.end()));
    if (select(mx + 1, &read_set_, NULL, NULL, NULL) < 0) {
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(sock_, &read_set_)) {
      size_ = sizeof(client_);
      new_sock_ = accept(sock_, (struct sockaddr *)&client_, &size_);
      if (new_sock_ < 0) {
        exit(EXIT_FAILURE);
      }
      fcntl(new_sock_, F_SETFL, O_NONBLOCK);
      clients.insert(new_sock_);
    }

    for (std::set<int>::iterator it = clients.begin(); it != clients.end();
         it++) {
      int clientSock = *it;
      if (FD_ISSET(*it, &read_set_)) {
        memset(&buf_, 0, sizeof(buf_));
        err_ = ReadFromClient(*it, buf_);
        if (err_ < 0) {
          close(*it);
          it = clients.erase(it);
          continue;
        }
        if (strstr(buf_, "stop")) {
          close(clientSock);
          it = clients.erase(it);
          continue;
        } else if (strstr(buf_, "count")) {
          WriteCountUser(*it);
        } else if (strstr(buf_, "message")) {
          std::string message_string(buf_);
          std::vector<std::string> message_param =
              SplitMessage(message_string, ' ');

          if (isValidMessage(message_param)) {
            int f = std::stoi(message_param[1]);
            if (clients.find(f) != clients.end()) {
              int user = *clients.find(f);
              WriteMessage(*it, user, message_param[2].c_str());
            } else {
              WriteErrorMessage(*it);
            }
          } else {
            WriteErrorMessage(*it);
          }

        } else if (strstr(buf_, "user_id")) {
          WriteAllUser(*it);
        } else {
          WriteToClient(*it, buf_);
        }
        memset(&buf_, 0, sizeof(buf_));
      }
    }
  }
}

int Server::ReadFromClient(int sockfd, char *buf) {
  int nbytes = 0;
  nbytes += recv(sockfd, buf, 2048, 0);
  if (nbytes < 0) {
    std::cout << "Error read";
    return -1;
  } else if (nbytes == 0) {
    std::cout << "No message";
  }

  return 0;
}

void Server::WriteToClient(int sockfd, char *buf) {
  int nbytes = 0;
  int count[256] = {0};
  char res_table[256];

  for (unsigned int i = 0; i < strlen(buf); i++) {
    if (buf[i] != ' ') {
      count[buf[i]]++;
    }
  }

  int pos = 0;
  for (int i = 0; i < 256; i++) {
    if (count[i] > 0) {
      pos += sprintf(res_table + pos, "%c %d\n", i, count[i]);
    }
  }

  nbytes += send(sockfd, res_table, strlen(res_table) + 1, 0);
}

void Server::WriteAllUser(int sockfd) {
  std::string res;
  for (std::set<int>::iterator user_it = clients.begin();
       user_it != clients.end(); user_it++) {
    char numstr[21];
    sprintf(numstr, "%d", *user_it);
    res += "user_id -" + std::string(numstr) + "\n";
  }
  send(sockfd, res.c_str(), strlen(res.c_str()) + 1, 0);
}

void Server::WriteMessage(int sockfd, int userfd, const char *mail) {
  char res[] = "Send message";
  send(userfd, mail, strlen(mail) + 1, 0);
  send(sockfd, res, strlen(res) + 1, 0);
}

void Server::WriteCountUser(int sockfd) {
  char count[2048];
  sprintf(count, "%s%ld", "the server has finished working ", clients.size());
  send(sockfd, count, strlen(count) + 1, 0);
}

void Server::WriteErrorMessage(int sockfd) {
  char count[2048] = "ERROR message id_user your text";
  send(sockfd, count, strlen(count) + 1, 0);
}

std::vector<std::string> Server::SplitMessage(const std::string &s,
                                              char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  int count = 0;
  while (getline(ss, item, delim)) {
    if (count < 3) {
      result.push_back(item);
    } else {
      result[2] += item + " ";
    }
    count++;
  }

  return result;
}

bool Server::isValidMessage(const std::vector<std::string> &substrings) {
  if (substrings.size() != 3) {
    return false;
  }
  if (substrings[0] != "message") {
    return false;
  }

  for (char c : substrings[1]) {
    if (!std::isdigit(c)) {
      return false;
    }
  }

  if (substrings[2].length() < 1 || substrings[2].length() > 2000) {
    return false;
  }

  return true;
}