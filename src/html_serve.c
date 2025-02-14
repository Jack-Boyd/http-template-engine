#include "html_serve.h"

char* serve_html(const char* filename) {
  //Open file in read mode (must exist, can only read it)
  FILE* file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return NULL;
  }
  
  //Sets file position to end of stream (0 from end of file)
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Error seeking to end of file");
    fclose(file);
    return NULL;
  }

  //Returns current file position (since jumped to end of file, will give the length)
  long length = ftell(file);
  if (length == -1) {
    perror("Error getting file size");
    fclose(file);
    return NULL;
  }

  //Sets file position to beginning of file 
  //(SEEK_SET is for proper use of offset/arg2, if set to 0, it's the beginning of the file)
  if (fseek(file, 0, SEEK_SET) != 0) {
    perror("Error seeking to beginning of file");
    fclose(file);
    return NULL;
  }

  //Make char pointer buffer to read file into C string
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