#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(8080);
  address.sin_addr.s_addr = INADDR_ANY;

  bind(server_socket, (struct sockaddr *)&address, sizeof(address));

  listen(server_socket, 5);

  int client_socket = accept(server_socket, NULL, NULL);

  char request[4096];
  int bytes_received = recv(client_socket, request, 4096, 0);

  std::stringstream stream(request);
  std::string line;
  while (std::getline(stream, line)) {
    std::cout << line << std::endl;
  }

  std::string html_document = "<HTML><BODY><H1> Hilsen Daniel: Du har koblet deg opp til min enkle web-tjener </h1>";

  std::string http_response = "HTTP/1.1 200 OK\r\n";
  http_response += "Content-Type: text/html\r\n";
  http_response += "Content-Length: " + std::to_string(html_document.length()) + "\r\n";
  http_response += "\r\n";
  http_response += html_document;

  send(client_socket, http_response.c_str(), http_response.length(), 0);

  close(client_socket);

  return 0;
}
