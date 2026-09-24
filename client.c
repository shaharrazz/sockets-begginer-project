#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
        int sock = 0;
        struct sockaddr_in serv_addr;
        char message[BUFFER_SIZE];
        char buffer[BUFFER_SIZE];

//create the socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0){
                perror("Socket crreation error");
                return -1;
        }

//client details
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if(inet_pton(AF_INET, "127.0.0.1",&serv_addr.sin_addr) <= 0){
                perror("Invalid address / Address not supported");
                return -1;
        }

//connect
        printf("[+] Connecting to server at 127.0.0.1:%d...\n",PORT);
        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
                perror("Connection failed");
                return -1;
        }

        printf("[+] Connected to Bot server! Type PING, TIME or EXIT.\n\n");

//conversation loop
        while (1){
                printf("You: ");
                fflush(stdout); //making sute print straight to screen
//reading message through terminal
                if(fgets(message, BUFFER_SIZE, stdin) == NULL){
                        break;
                }
//sending message to server
                send(sock, message, strlen(message), 0);
//STARTUP THE BUFFER
                memset(buffer,0, BUFFER_SIZE);
                ssize_t bytes_read = recv(sock, buffer, BUFFER_SIZE -1,0);

                if (bytes_read > 0){
                        buffer[bytes_read] = '\0';
                        printf("[Bot]: %s",buffer);
                        if (strncmp(message, "EXIT",4) == 0){
                                break;
                        }
                } else {
                        printf("[-] Server disconnected.\n");
                        break;
                }
        }
//close sockets
        close(sock);
        printf("[+] Connection closed.\n");
        return 0;
}



