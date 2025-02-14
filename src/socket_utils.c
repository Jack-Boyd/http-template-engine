#include "socket_utils.h"

int initialize_server(struct sockaddr_in* address) {
  int server_fd;
  int opt = 1;
  //Create socket endpoint
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket failed!");
    return -1;
  }

  //Set socket to reuse address
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    close(server_fd);
    return -1;
  }

  //Set socket to reuse port 
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    perror("setsockopt(SO_REUSEPORT) failed");
    close(server_fd);
    return -1;
  }

  //Set address struct to values to local 
  address->sin_family = AF_INET; 
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);

  //Bind socket to created address
  if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
    perror("Bind failed!"); 
    close(server_fd);
    return -1;
  }

  //Listen on socket for at most 3 connections 
  if (listen(server_fd, 3) < 0) {
    perror("Listen failed!");
    close(server_fd);
    return -1;
  }

  return server_fd;
}

void read_client_data(int socket, char* buffer) {
  ssize_t read_value;

  while((read_value = read(socket, buffer, BUFFER_SIZE)) > 0) {
    printf("Client: %s", buffer);
    memset(buffer, 0, BUFFER_SIZE);
  }
}