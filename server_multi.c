#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h> //thread laibary
#include <time.h>
#include <openssl/evp.h>

#include <openssl/ssl.h>
#include <openssl/err.h>



#define PORT 8080
#define BUFFER_SIZE 1024
#define LOG_FILE "server.log"
#define MAX_REQUESTS_PER_SEC 5

#define VALID_USERNAME "shahar"
#define SALT "c3k9_CyberSec2026!$"
// SHA256("Secret123!" + "c3k9_CyberSec2026!$")
//PASSWORD  Secret123!
#define VALID_PASSWORD_HASH "d2447d916f9f2228fcff669ff3a52d4fb090751202233e2c8d20cc533c8d8cef"

#define MAX_FAILED_ATTEMPTS 3
#define LOCKOUT_TIME_SEC 30

int failed_attempts = 0;
time_t lockout_start_time = 0;
pthread_mutex_t auth_lock_mutex = PTHREAD_MUTEX_INITIALIZER;

void computer_sha256_with_salt(const char *password, const char *salt, char output_hex[65]){
        unsigned char hash[EVP_MAX_MD_SIZE]; //lock safe for threads
        unsigned int hash_len; //lock safe for logging

        char salted_input[256] = {0};//buffer to put the salt

        snprintf(salted_input, sizeof(salted_input), "%s%s", password, salt);

        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

        EVP_DigestUpdate(ctx,salted_input, strlen(salted_input));
        EVP_DigestFinal_ex(ctx, hash,&hash_len);
        EVP_MD_CTX_free(ctx);

        for (unsigned int i=0; i< hash_len; i++) {
                sprintf(output_hex + (i*2), "%02x", hash[i]);
        }
        output_hex[64] = '\0';
}



int active_clients = 0; //COUNTING CLIENTS
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; //lock safe fot threads
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

//log function for thread
void log_event(const char *level, const char *message){
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%y-%m-%d %H:%M:%S",t);

        pthread_mutex_lock(&log_mutex);
//print to screen
        printf("[%s] [%s] %s\n", time_str, level,message);
//create and open file
        FILE *f = fopen(LOG_FILE, "a");
        if (f !=NULL){
                fprintf(f, "[%s] [%s] %s\n", time_str, level,message);
                fclose(f);
        }
        pthread_mutex_unlock(&log_mutex);
}

void *handle_client(void *client_socket_ptr){
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout server.key -out server.crt  int new_socke>
        free(client_socket_ptr); //free memory for socket id

        char buffer[BUFFER_SIZE] = {0};
        char log_buff[256];

        int message_count = 0;
        time_t window_start = time(NULL);

        int is_authenticated = 0;

//updates when client connects
       pthread_mutex_lock(&clients_mutex);
        active_clients++;
        snprintf(log_buff, sizeof(log_buff), "[Thread %ld] Client connected on sock %d. Active client>
 (long)pthread_self(), new_socket, active_clients);
        log_event("INFO", log_buff);
        pthread_mutex_unlock(&clients_mutex);

        char *welcome_msg = "Welcome to server! \nPlease LOGIN to continue. Usage: LOGIN <username> <>
        send(new_socket, welcome_msg, strlen(welcome_msg), 0);

//conversation loop with bot

        while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);

                if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
//check rate lomitng
                time_t current_time = time(NULL);
                printf("[Socket %d]: %s", new_socket, buffer);

                if (current_time - window_start >= 1){
//if more than 1 second
                        window_start = current_time;
                        message_count = 1;
                }else{
                        message_count++;
                        if (message_count > MAX_REQUESTS_PER_SEC){
                                snprintf(log_buff, sizeof(log_buff), "SECURITY ALERT: Socket %d excee>
                                ,new_socket, message_count);
                                log_event("WARNING", log_buff);

                                char *err_msg = "HTTP/1.1 429 Too Many Requests\nRate limit exceeded.>
                                send(new_socket, err_msg, strlen(err_msg), 0);
                                break;
                        }
                }
//comand login

                if (strncmp(buffer, "LOGIN ", 6) == 0){
                        char username[64] = {0};
                        char password[64] = {0};

                        pthread_mutex_lock(&auth_lock_mutex);
                        time_t now = time(NULL); //getting current time
                        //checking max tries
                       if(failed_attempts >= MAX_FAILED_ATTEMPTS){
                                //cac how many seconds left out of 30
                                double seconds_left = LOCKOUT_TIME_SEC - difftime(now, lockout_start_>

                                if (seconds_left >0 ){
                                        pthread_mutex_unlock(&auth_lock_mutex);
                                        char reply[128];
                                        snprintf(reply, sizeof(reply), "Account locked due to mutiple>
                                                ,seconds_left);
                                        send(new_socket, reply, strlen(reply), 0);

                                        snprintf(log_buff, sizeof(log_buff), "SECURITY ALLERT: Reject>
                                        log_event("WARNING", log_buff);

                                        continue;
                                }else{
                                // after 30 seconds- reset
                                failed_attempts = 0;
                                lockout_start_time = 0;
                                log_event("INFO", "Lockout period expierd. Accound now is unlocked");
                                }
                        }
                        pthread_mutex_unlock(&auth_lock_mutex);

                        if (sscanf(buffer + 6, "%63s %63s", username, password) == 2) {
                                password[strcspn(password, "\r\n")] = 0;
                                username[strcspn(username, "\r\n")] = 0;

                                char computed_hash[65] = {0};
                                 computer_sha256_with_salt(password,SALT, computed_hash);



                                        printf("\n[DEBUG] Username received: '%s'\n", username);
                                        printf("[DEBUG] Salt used: '%s'\n", SALT);
                                        printf("[DEBUG] Salted Computed Hash: '%s'\n", computed_hash);
                                        printf("[DEBUG] Expected Hash: '%s'\n\n", VALID_PASSWORD_HASH>



                                if (strcmp(username, VALID_USERNAME) == 0 && strcmp(computed_hash,VAL>
                                        //reset when connection succsided
                                        pthread_mutex_lock(&auth_lock_mutex);
                                        failed_attempts = 0;
                                        pthread_mutex_unlock(&auth_lock_mutex);

                                        is_authenticated = 1;
                                        char *reply = "200 SUCCESS: Authenticated successfully!\n";
                                        send(new_socket, reply, strlen(reply), 0);

                                        snprintf(log_buff, sizeof(log_buff), "User '%s' authenticated>
                                        log_event("INFO", log_buff);
                                }else{
                                        // dealing with failed attempts
                                        pthread_mutex_lock(&auth_lock_mutex);
                                        failed_attempts++;

                                        //check if lock
                                        if (failed_attempts >= MAX_FAILED_ATTEMPTS){
                                                lockout_start_time = time(NULL);
                                                snprintf(log_buff, sizeof(log_buff), "SECURITY ALERT:>
                                                         MAX_FAILED_ATTEMPTS, LOCKOUT_TIME_SEC);
                                                log_event("WARNING", log_buff);
                                        }else{
                                        snprintf(log_buff,sizeof(log_buff),"SECURITY WARNING: Failed >
                                                         failed_attempts, MAX_FAILED_ATTEMPTS, userna>
                                        log_event("WARNING", log_buff);
                                        }
                                        pthread_mutex_unlock(&auth_lock_mutex);

                                        is_authenticated = 0;
                                        char reply[128];
                                        //showing how many attempts
                                        snprintf(reply, sizeof(reply), "401 UNAUTHORIZED: Invalid cre>
                                                         failed_attempts, MAX_FAILED_ATTEMPTS);
                                        send(new_socket, reply, strlen(reply),0);
                                }
                        } else{
                                char *reply = "401 UNAUTHORIZED: Invalid credentials!\n";
                                send(new_socket, reply, strlen(reply), 0);

                        }

            }else if (strncmp(buffer, "PING", 4) == 0) {
                if (!is_authenticated){
                        char *reply = "FORBIDDEN: Please LOGIN first\n";
                         send(new_socket, reply, strlen(reply), 0);
                }else{
                char *reply = "PONG!\n";
                send(new_socket, reply, strlen(reply), 0);
                }
            } else if (strncmp(buffer, "TIME", 4) == 0) {
                if (!is_authenticated){
                        char *reply = "FORBIDDEN: Please LOGIN first\n";
                         send(new_socket, reply, strlen(reply), 0);
                }else{
                        char *reply = "Server time: Active Multi-Threaded Session!\n";
                        send(new_socket, reply, strlen(reply), 0);
                }

            } else if (strncmp(buffer, "EXIT", 4) == 0) {
                char *reply = "Goodbye!\n";
                send(new_socket, reply, strlen(reply), 0);
                break;

            } else {
                char *reply = "Unknown command. Try PING, TIME, or EXIT.\n";
                send(new_socket, reply, strlen(reply), 0);
            }

        } else if (bytes_read == 0) {
                snprintf(log_buff, sizeof(log_buff),"Socket %d disconnected gracefully.\n", new_socke>
                log_event("INFO", log_buff);
                break;
        } else {
            perror("Recv failed");
            break;
        }
    }
//updates when client disconnect
        close(new_socket);
        pthread_mutex_lock(&clients_mutex);
        active_clients--;
        snprintf(log_buff, sizeof(log_buff),"[-] [Thread %ld] Client disconnected. Active clients con>
 (long)pthread_self(), active_clients);
        pthread_mutex_unlock(&clients_mutex);

        return NULL;
}

int main(){
        int server_fd;
        struct sockaddr_in address;

        log_event("INFO", "Startig Multi-Thread Cyber Server...");

// create the socket
        server_fd = socket(AF_INET, SOCK_STREAM,0);
        if (server_fd == 0){
                perror("Socket failed");
                exit(EXIT_FAILURE);
        }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET ,SO_REUSEADDR, &opt, sizeof(opt));

//bind
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) <0 ){
                perror("Bind failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        printf("[*] Multi-threaded server listing on port %d...\n", PORT);


// Listen
        if (listen(server_fd, 10) < 0) {
                perror("Listen failed");
                close(server_fd);
                exit(EXIT_FAILURE);
        }
        char start_log[128];
        snprintf(start_log, sizeof(start_log), "Multi-threaded server listening on port %d...\n", POR>
        log_event("INFO", start_log);

//CREATING THREATS LOOP
        while (1){

                socklen_t addrlen = sizeof(address);

                int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
                if (new_socket < 0){
                        perror("Accept failed");
                        continue;//keep listen after no connect
                }
                printf("\n[+] New connection accepted! Creating thread...\n");

//use pointers with another client
                int *new_sock_ptr = malloc(sizeof(int));
                *new_sock_ptr = new_socket;

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




}

