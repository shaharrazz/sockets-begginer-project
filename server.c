                                server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //close()
#include <arpa/inet.h> //struct sockaddr_in

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        char buffer[BUFFER_SIZE] = {0}; //buffer to storage client message

//create the socket
// AF_INET = Ipv4 SOCK_STREAM = TCP
        server_fd = socket(AF_INET, SOCK_STREAM,0);
        if (server_fd == 0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
        }
        printf("[+] Socket created successfully.\n");

//sulotion for "address already in use"
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//server address and port define
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

//connect the bind tosocket address and port
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
                perror("Bind failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        printf("[+] Bound to port %d successfully.\n", PORT);

//listen (up to 3 connects)
        if (listen(server_fd,3)<0){
                perror("Listen failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        printf("[*] Bot Server listening on port %d...\n",PORT);

//Accept
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket <0){
                perror("Accept Failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        printf("[+] Client connected to Bot Server!/n");

//loop for long conversation
        while (1){
        //restart the buffer
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t bytes_read = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
                if (bytes_read > 0){
                //adding ending char\n
                        buffer[bytes_read] = '\0';
                        printf("[Client]: %s", buffer);
                //Bot
                        if (strncmp(buffer, "PING",4) == 0){
                                char *reply = "PONG!\n";
                                send(new_socket, reply, strlen(reply), 0);
                        } else if (strncmp(buffer, "TIME", 4) == 0){
                                char *reply = "Server time: 2026 Session Activate!\n";
                                send(new_socket, reply, strlen(reply),0);
                        } else if (strncmp(buffer, "EXIT",4 ) == 0){
                                char *reply = "Goodbye!\n";
                                send(new_socket, reply, strlen(reply),0);
                                printf("[+] Client sent EXIT. Closing connection...\n");
                                break;
                        } else {
                                char *reply= "Unknown command. Try PING, TIME or EXIT.\n";
                                send(new_socket, reply,strlen(reply), 0);
                        }

                } else if (bytes_read == 0){
                        printf("\n[-] Client disconnected gracefully.\n");
                        break;
                }else{
                        perror("Recv failed");
                        break;
                }
        }
//close the sockets
        close(new_socket);
        close(server_fd);
        printf("[+] Connection closed./n");

        return 0;
}
