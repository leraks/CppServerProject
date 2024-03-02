#include "client.h"

int main() {
    Client client;
    client.SocketStart();
    client.Connect();
    client.ClientWork();
    return 0;
}