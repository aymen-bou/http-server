#include <arpa/inet.h>
#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

// #include "file_utils.h"

#define CONFIG_PATH "server_config.text"
#define LOCALHOST "127.0.0.1"

typedef struct {
  uint16_t port;
} ServerConfig;

ServerConfig load_server_config(const char *path);

int main() {
  ServerConfig config = load_server_config(CONFIG_PATH);

  struct sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(config.port);
  server_addr.sin_addr.s_addr = inet_addr(LOCALHOST);

  printf("Starting server on IP: %s, PORT: %d\n",
         inet_ntoa(server_addr.sin_addr), config.port);

  int32_t server_socket_fd;

  if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
      exit(EXIT_FAILURE);
  }
  printf("Socket file descriptor: %d\n",
         server_socket_fd); // Should print 3

  int32_t bind_status = bind(server_socket_fd, (struct sockaddr *)&server_addr,
                         sizeof(server_addr));

  if (bind_status < 0) {
    perror("Error binding socket");
    close(server_socket_fd);
      exit(EXIT_FAILURE);
  }
  printf("Socket successfully bound to port %d\n", config.port);

  if (listen(server_socket_fd, SOMAXCONN) < 0) {
    perror("Error listening on socket");
    close(server_socket_fd);
      exit(EXIT_FAILURE);
  }

  printf("Socket is now listening on port %d and will accept incoming "
         "connections.\n",
         config.port);

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  while (1) {
    int32_t client_socket_fd = accept(
        server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_socket_fd < 0) {
      perror("accept failed");
      continue;
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // for testing purposes
    char buffer[1024];
    int32_t bytes_read;

    while ((bytes_read = read(client_socket_fd, buffer, sizeof(buffer) - 1)) >
           0) {
      printf("Received Data from client with %s:%d > %s",
             inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
             buffer);
    }

    close(client_socket_fd);
  }

  














  return 0;
}

ServerConfig load_server_config(const char *path) {
    printf("Start Loading server configuration...\n");
    FILE *config_file = fopen(path, "r");
    if (config_file == NULL) {
      perror("Failed to open configuration file");
      exit(EXIT_FAILURE);
    }

    ServerConfig config = {0}; // default init
    char line[256];

    while (fgets(line, sizeof(line), config_file)) {
      int32_t port;
      if (sscanf(line, "port=%d", &port) == 1) {
        config.port = port;
        printf("Server configuration loaded Successfully.\n");
      }
    }

    fclose(config_file);

    if (config.port == 0) {
      fprintf(stderr, "Error: 'port' not set in %s\n", path);
      exit(EXIT_FAILURE);
    }

    return config;
  }