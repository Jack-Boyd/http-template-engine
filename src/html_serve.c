#include "html_serve.h"

char* serve_html(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return NULL;
  }
  
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Error seeking to end of file");
    fclose(file);
    return NULL;
  }
  long length = ftell(file);
  if (length == -1) {
    perror("Error getting file size");
    fclose(file);
    return NULL;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Error seeking to beginning of file");
    fclose(file);
    return NULL;
  }

  char* buffer = malloc(length + 1);
  if (!buffer) {
    perror("Error allocating memory");
    fclose(file);
    return NULL;
  }

  if (fread(buffer, 1, length, file) != length) {
    perror("Error reading file");
    fclose(file);
    return NULL;
  }
  buffer[length] = '\0';
  fclose(file);
  return buffer;
}