#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

class Client {
public:
  int sock, readval, server_socket;
  struct sockaddr_in serv_addr;
  char buffer[1024];

  int start() {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket creation failed");
      exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
      perror("Address invalid");
      exit(EXIT_FAILURE);
    }

    if ((server_socket = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)) {
      perror("Connection failed");
      exit(EXIT_FAILURE);
    }

    return 1;
  }

  string readInput() {
    readval = read(sock, buffer, 1024);
    string received = "";
    for (char a : buffer) {
      if (a == '#') {
        break;
      }
      received.append(1, a);
    }
    memset(buffer, 1024, '\0');
    return received;
  }

  int sendOutput(string message) {
    message.append(1, '#');
    const char *out = message.c_str();
    return send(sock, out, strlen(out), 0);
  }
  void stop() {
    close(server_socket);
  }
};

int main() {
  cout << "Client side is running" << endl;
  Client client = Client();
  client.start();
  string message = "Socket open";
  cout << client.readInput() << endl;
  client.sendOutput(message);

  while (true) {
    cout << client.readInput() << endl;

    string in;
    cin >> in;
    message = in;
    client.sendOutput(message);

    cout << client.readInput() << endl;
    in = "";

    cin >> in;
    message = in;
    client.sendOutput(message);

    cout << client.readInput() << endl;
    in = "";
    cin >> in;
    message = in;
    client.sendOutput(message);

    cout << client.readInput() << endl;
  }
  client.stop();
}
