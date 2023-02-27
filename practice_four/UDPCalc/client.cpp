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

class Client {
public:
  int sockfd;
  char buffer[1024];
  struct sockaddr_in addr;

  int start() {
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket creation failed");
      exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    return 1;
  }

  void send(string message) {
    const char *out = message.c_str();
    sendto(sockfd, (const char *)out, strlen(out), MSG_CONFIRM, (const struct sockaddr *)&addr, sizeof(addr));
  }

  void read() {
    int n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));
    buffer[n] = '\0';
  }

  void stop() {
    close(sockfd);
  };
};

int main() {
  cout << "Client side is running" << endl;
  Client client = Client();
  client.start();
  string message = "Socket open";
  cout << "Calculator is now working.\nEnter some math expression in a term like this a+b. Input x to exit" << endl;
  while (1) {
    string exp;
    cin >> exp;
    if (!exp.compare("x")) {
      break;
    }
    client.send(exp);
    client.read();
    cout << client.buffer << endl;
  }
  client.stop();
}
