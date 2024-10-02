#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, sizeof(buffer) - 1);
    
    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");

    if (strcmp(method, "GET") == 0) {
        if (path == NULL || strcmp(path, "/") == 0) {
            path = "index.html";
        } else {
            path++;
        }

        FILE *file = fopen(path, "r");
        if (file) {
            char response[BUFFER_SIZE];
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", file_size);
            send(client_socket, response, strlen(response), 0);
            fread(response, 1, file_size, file);
            send(client_socket, response, file_size, 0);
            fclose(file);
        } else {
            char *response = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
            send(client_socket, response, strlen(response), 0);
        }
    }
    
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
