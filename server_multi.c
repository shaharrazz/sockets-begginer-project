#include #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h> //thread laibary

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_client(void *client_socket_ptr){
        int new_socket = *(int *)client_socket_ptr;
        free(client_socket_ptr); //free memory for socket id

        char buffer[BUFFER_SIZE] = {0};

        PRINTF("[THREAD %ld] Handeling client socket %d\n", (long)pthread_self(), new_socket);

//conversation loop with bot

        while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[Socket %d]: %s", new_socket, buffer);

            if (strncmp(buffer, "PING", 4) == 0) {
                char *reply = "PONG!\n";
                send(new_socket, reply, strlen(reply), 0);

            } else if (strncmp(buffer, "TIME", 4) == 0) {
                char *reply = "Server time: Active Multi-Threaded Session!\n";
                send(new_socket, reply, strlen(reply), 0);

            } else if (strncmp(buffer, "EXIT", 4) == 0) {
                char *reply = "Goodbye!\n";
                send(new_socket, reply, strlen(reply), 0);
                break;

            } else {
                char *reply = "Unknown command. Try PING, TIME, or EXIT.\n";
                send(new_socket, reply, strlen(reply), 0);
            }

        } else if (bytes_read == 0) {
            printf("[-] Socket %d disconnected gracefully.\n", new_socket);
            break;
        } else {
            perror("Recv failed");
            break;
        }
    }
//close and restart the thread
        close(new_socket);
        printf("[Thread %d] Exiting, closed socket %d.\n", (long)pthread_self(), new_socket);
        return NULL;
}

int main(){
        int server_fd;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
// create the socket
        server_fd = socket(AF_INET, SOCK_STREAM,0);
        if (server_fd == 0){
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }

        int opt = 1;
        setsockopt(server_fd, SOL_REUSEADDR, &opt, sizeof(opt));

//bind
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address, sizrof(address)) <0 ){
                perror("Bind failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        printdf("[*] Multi-threaded server listing on port %d...\n", PORT);

//CREATING THREATS LOOP
        while (1){
                int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&address>
                if (new_socket < 0){
                        perror("Accept failed");
                        continue;//keep listen after no connect
                }
                printf("\n[+] New connection accepted! Creating thread...'n");

//use pointers with another client
                int *new_sock_ptr = malloc(sizeof(int));
                *new_sock_ptr = nwe_socket;

                pthread_t thread_id;
                if (pthread_create(&thread_id, NULL, handle_client, (void*) new_sock_ptr) <0){
                        perror("Could not create thread");
                        free(new_sock_ptr);
                        close(new_socket);
                }

                pthread_detach(thread_id);
        }
close(server_fd);
return 0;


