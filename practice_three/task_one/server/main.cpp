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
  struct sockaddr_in address;
  int server_socket;
  int opt = 1;
  int addrlen = sizeof(address);

  int start() {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }
    if (setsockopt(server_socket, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("socket bind failed");
      exit(EXIT_FAILURE);
    }

    while (1) {
      int newsocket = 1;
      struct sockaddr_in newaddress;
      int newlen = sizeof(newaddress);

      if (listen(server_socket, 3) < 0) {
        perror("listen for connection failed");
        exit(EXIT_FAILURE);
      }

      if ((newsocket = accept(server_socket, (struct sockaddr *)&newaddress, (socklen_t *)&newlen)) < 0) {
        perror("accept connection failed");
        exit(EXIT_FAILURE);
      }

      thread t(&Server::handleConnection, this, newsocket);
      t.detach();
    }
  }

  void handleConnection(int socketid) {
    char buffer[1024];

    string message = "Socket open";
    int sent = sendOutput(message, socketid);
    cout << readInput(socketid, buffer) << endl;

    while (true) {

      message = "Input the first number";
      sent = sendOutput(message, socketid);

      int numone = readInt(socketid, buffer);
      cout << numone << endl;

      message = "Input the second number";
      sent = sendOutput(message, socketid);

      int numtwo = readInt(socketid, buffer);
      cout << numtwo << endl;

      message = "Select math function \n1.+ \n2.- \n3.* \n4./ ";
      sendOutput(message, socketid);

      int math = readInt(socketid, buffer);
      cout << math << endl;

      float sum;
      switch (math) {
      case 1:
        sum = numone + numtwo;
        break;
      case 2:
        sum = numone - numtwo;
        break;
      case 3:
        sum = numone * numtwo;
        break;
      case 4:
        sum = (float)numone / (float)numtwo;
        break;
      }

      message = to_string(sum);
      sendOutput(message, socketid);
      this_thread::sleep_for(500ms);
    }
  }

  string readInput(int client_socket, char (&buffer)[1024]) {
    int readval;
    readval = read(client_socket, buffer, 1024);
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

  int readInt(int client_socket, char (&buffer)[1024]) {
    int readval;
    readval = read(client_socket, buffer, 1024);
    string received = "";
    for (char a : buffer) {
      if (a == '#') {
        break;
      }
      received.append(1, a);
    }
    try {
      int numone = stoi(received);
      memset(buffer, 1024, '\0');
      return numone;
    } catch (exception const &e) {
      cout << e.what() << endl;
    }
    memset(buffer, 1024, '\0');
    return -1;
  }

  int sendOutput(string message, int client_socket) {
    message.append(1, '#');
    const char *out = message.c_str();
    return send(client_socket, out, strlen(out), 0);
  }

  void stop(int client_socket) {
    close(client_socket);
    shutdown(server_socket, SHUT_RDWR);
  }
};

int main() {
  Server server = Server();
  server.start();
}
