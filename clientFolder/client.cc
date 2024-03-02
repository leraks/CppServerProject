#include "client.h"

Client::Client() {
  hostinfo_ = gethostbyname(SERVER_NAME);

  server_addr_.sin_family = AF_INET;
  server_addr_.sin_port = htons(SERVER_PORT);
  server_addr_.sin_addr = *(struct in_addr*)hostinfo_->h_addr;
}

Client::~Client() {
  if (sock_open_ == 1) {
    close(sock_);
  }
}

void Client::SocketStart() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    std::cout << "Error socket" << std::endl;  // TROW броботать шибку!!
  }
  sock_open_ = 1;
}

void Client::Connect() {
  connect_err_ =
      connect(sock_, (struct sockaddr*)&server_addr_, sizeof(server_addr_));
  if (connect_err_ < 0) {
    std::cout << "Error connect" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Client::ClientWork() {
  while (1) {
    if (WriteToServer(sock_) < 0) break;
    if (ReadFromServer(sock_) < 0) break;
  }
}

int Client::WriteToServer(int sockfd) {
  std::string data;
  std::thread inputThread(readInput, std::ref(data));

  inputThread.join();
  if (data.length() > 0) {
    char buf[2048];
    int nbytes = 0;
    strcpy(buf, data.c_str());
    if (data.length() > 0) {
      nbytes += send(sockfd, buf, strlen(buf) + 1, 0);
    }

    if (strstr(buf, "stop")) {
      return -1;
    }
  }
  return 0;
}

int Client::ReadFromServer(int sockfd) {
  std::thread receiveThread(ReceiveMessages, sockfd);
  receiveThread.detach();
  return 0;
}

void Client::readInput(std::string& data) { std::getline(std::cin, data); }

void Client::ReceiveMessages(int sockfd) {
  char buf[2048];
  while (true) {
    int nbytes = recv(sockfd, buf, 2048, 0);
    if (nbytes > 0) {
      std::cout << "Server response: " << buf << std::endl;
    }
  }
}