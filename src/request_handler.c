#include "request_handler.h"

void handle_client(int new_socket) {
  char buffer[BUFFER_SIZE] = {0};

  read(new_socket, buffer, BUFFER_SIZE);
  
  if (strstr(buffer, "GET /") != NULL && strstr(buffer, "GET /favicon.ico") == NULL) {
    printf("Connection accepted\n");
    printf("Request: %s\n", buffer);
  }

  if (strstr(buffer, "GET /favicon.ico") != NULL) {
    close(new_socket);
    return;
  }
  char* html_content = serve_html("../html/index.html");
  if (html_content) {
    const char* keys[] = {"user", "is_logged_in"};
    const char* values[] = {"Dexter", "1"};
    const char* loop_key = "item";
    const char* loop_values[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
    char* processed = process_template(
        html_content, 
        keys, values, 2,
        loop_key, loop_values, 4
    );
    char headers[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n\r\n";
                    
    write(new_socket, headers, strlen(headers));
    write(new_socket, processed, strlen(processed));

    free(html_content);
    free(processed);
  } else {
    const char* error = "HTTP/1.1 404 Not Found\r\n"
                       "Content-Type: text/html\r\n\r\n"
                       "<h1>404 Not Found</h1>";
    write(new_socket, error, strlen(error));
  }

  close(new_socket);
}