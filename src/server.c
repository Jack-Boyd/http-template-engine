#include "server.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  //Create socket on port 8080 
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  
  server_fd = initialize_server(&address);
  if (server_fd == -1) {
    return EXIT_FAILURE;
  }

  printf("Server listening on port: %d\n", PORT);

  while(1) {
    //Accept incoming connections to socket on port
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&address);

    if (new_socket < 0) {
      perror("Connection not accepted: ");
      continue;
    }

    handle_client(new_socket);
  }

  close(server_fd);
  return 0;
}