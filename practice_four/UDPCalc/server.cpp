#include "math_parser.h"
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std;

class Server {
public:
  struct sockaddr_in serveradr, clientadr;
  int server_socket;
  char buffer[1024];

  int start() {
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

    serveradr.sin_family = AF_INET;
    serveradr.sin_addr.s_addr = INADDR_ANY;
    serveradr.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&serveradr, sizeof(serveradr)) < 0) {
      perror("socket bind failed");
      exit(EXIT_FAILURE);
    }

    cout << "Socket initialized, calculator open" << endl;

    socklen_t len;
    len = sizeof(serveradr);
    while (1) {

      int n = recvfrom(server_socket, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&clientadr, &len);
      buffer[n] = '\0';
      cout << buffer << endl;

      double ans = MathParser::parse(convertToString(buffer, n));
      cout << ans << endl;
      const char *a = to_string(ans).c_str();
      sendto(server_socket, (const char *)a, strlen(a), MSG_CONFIRM, (const struct sockaddr *)&clientadr, len);
    }
  }

  string convertToString(char *a, int size) {
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
      s = s + a[i];
    }
    return s;
  }

  void send(string message, int socket, struct sockaddr_in addr) {
    const char *out = message.c_str();
    sendto(socket, (const char *)out, strlen(out), 0, (const struct sockaddr *)&addr, sizeof(addr));
  }

  void read(int socket, char *buffer, struct sockaddr_in addr) {
    int n = recvfrom(socket, (char *)buffer, 1024, 0, (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));
    buffer[n] = '\0';
  }
  int readInt(int socket, char *buffer, struct sockaddr_in addr) {
    int n = recvfrom(socket, (char *)buffer, 1024, 0, (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));
    buffer[n] = '\0';
    try {
      int numone = stoi(buffer);
      return numone;
    } catch (exception const &e) {
      cout << e.what() << endl;
    }
    return -1;
  }
  void stop() {
    close(server_socket);
  };
};

int main() {
  cout << "Starting server" << endl;
  Server server = Server();
  server.start();
  server.stop();
}
