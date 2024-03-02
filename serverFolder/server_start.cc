#include "server.h"

int main() {
  Server server;
  server.SocketStart();
  server.BindStart();
  server.ListenStartAndFdSet(3);
  server.ServerWork();
  return 0;
}