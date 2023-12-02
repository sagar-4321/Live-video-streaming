#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

void error_handler(char *message) 
 {
  perror(message);
  exit(1); // process termination
 }

void receive_file(FILE *file, int socket)
 {
  char buffer[MAX_BUFFER_SIZE];
  ssize_t bytesRead; // Used for getting the count of bytes

  struct sockaddr_in client_address;
  socklen_t client_address_len = sizeof(client_address);

  while (1) 
  {
    bytesRead = recvfrom(socket, buffer, sizeof(buffer), 0,(struct sockaddr *)&client_address, &client_address_len);

    if (bytesRead == -1) 
    {
      error_handler("recvfrom() failed");
    }

    if (bytesRead == 0)
    {
      break; // End of file reached
    }
    
    fwrite(buffer, 1, bytesRead, file);
   }
 }
 
int main(int argc, char *argv[]) 
{
  if (argc != 5) 
  {
    fprintf(stderr, "Usage: %s <output_filename1> <output_filename2> <output_filename3> <listening_port>\n", argv[0]);
    exit(1);
  }

  char *output_filename1 = argv[1];
  char *output_filename2 = argv[2];
  char *output_filename3 = argv[3];
  
  int listening_port = atoi(argv[4]);

  FILE *file1 = fopen(output_filename1, "wb");
  FILE *file2 = fopen(output_filename2, "wb");
  FILE *file3 = fopen(output_filename3, "wb");

  if (!file1 || !file2 || !file3) 
  {
    error_handler("Unable to create output file");
  }

  int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_socket == -1) 
  {
    error_handler("socket() failed");
  }

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  //server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_port = htons(listening_port);

  if (bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) 
  {
    error_handler("bind() failed");
  }

  receive_file(file1, udp_socket);
  receive_file(file2, udp_socket);
  receive_file(file3, udp_socket);

  fclose(file1);
  fclose(file2);
  fclose(file3);
  close(udp_socket);

  return 0;
}

