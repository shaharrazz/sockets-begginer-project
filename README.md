# 🔒 Multi-Threaded Secure TLS/SSL Cyber Server (C Implementation)

A high-performance, secure, and multi-threaded TCP server built in **C** using **POSIX Threads (`pthread`)** and **OpenSSL**. 

This project demonstrates core cybersecurity and networking concepts, including end-to-end TLS 1.3 encryption, salted SHA-256 password hashing, rate limiting against Denial of Service (DoS) attacks, brute-force lockout protection, and thread-safe concurrent connection handling.

---

## ✨ Features

- **End-to-End Encryption (TLS/SSL):** Secured communication channel using OpenSSL (`SSL_read`, `SSL_write`), preventing eavesdropping and Man-in-the-Middle (MitM) attacks.
- **Multi-Threaded Architecture:** Handles multiple concurrent client connections asynchronously using `pthread_create` and `pthread_detach`.
- **Salted SHA-256 Password Hashing:** Cryptographically secure authentication mechanism using OpenSSL's `EVP` API with custom dynamic salting.
- **Brute-Force & Account Lockout Protection:** Automatically locks user authentication after **3 failed login attempts** for a duration of **30 seconds**.
- **Rate-Limiting (Anti-DoS/Flooding):** Restricts clients to a maximum of **5 requests per second**. Exceeding the threshold triggers an HTTP `429 Too Many Requests` warning and drops the socket.
- **Thread-Safe Logging System:** Real-time console and file log (`server.log`) synchronization protected by `pthread_mutex_t`.

---

## 🏗️ Architecture & Security Design
+------------------+         Encrypted TLS Channel         +----------------------+
|  OpenSSL Client  | <===================================> | Multi-Thread Server  |
|  (s_client)      |        (SSL_read / SSL_write)         | (OpenSSL + pthreads) |
+------------------+                                       +----------------------+
|
+-----------+-----------+
|                       |
[ Mutex Lock ]          [ Mutex Lock ]
|                       |
Rate Limit Check       Account Lockout &
& Logging System        Salted Hash Auth

---

## 🛠️ Prerequisites

Ensure you have the required compilers and OpenSSL libraries installed on your Linux machine (Ubuntu/Debian/WSL):

sudo apt update
sudo apt install build-essential libssl-dev openssl -y

Getting Started
1. Clone the Repository

git clone [https://github.com/your-username/secure-tls-server.git](https://github.com/your-username/secure-tls-server.git)
cd secure-tls-server

2. Generate Self-Signed SSL Certificates
Generate a private key (server.key) and a self-signed certificate (server.crt):

openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout server.key \
  -out server.crt \
  -subj "/C=IL/ST=Center/L=BneiBrak/O=ITQ/OU=CyberSec/CN=127.0.0.1"

3. Compile the Server
Compile the code with threading (-lpthread) and OpenSSL (-lcrypto -lssl) libraries:
gcc server_multi.c -o server_multi -lpthread -lcrypto -lssl

4. Run the Server
   
./server_multi

Connecting with OpenSSL Client
Open a separate terminal window and connect to the secure server using openssl s_client:

openssl s_client -connect 127.0.0.1:8080 -crlf -quiet

Command,Usage,Description,Authentication Required
LOGIN,LOGIN <username> <password>,Authenticates the user via Salted SHA-256 hash matching.,❌ No
PING,PING,Health check command; responds with PONG!.,✅ Yes
TIME,TIME,Returns server session status/time info.,✅ Yes
EXIT,EXIT,Safely closes the TLS connection.,❌ No

Security Incident Log Simulation (server.log)
All administrative events and security breaches are tracked thread-safely in server.log:
[2026-10-05 14:15:01] [INFO] Starting Multi-Thread Secure SSL/TLS Cyber Server...
[2026-10-05 14:15:10] [INFO] [Thread 1398711] Secure TLS client connected on sock 4. Active: 1
[2026-10-05 14:15:22] [WARNING] SECURITY WARNING: Failed login attempt (1/3) for user 'shahar'
[2026-10-05 14:15:25] [WARNING] SECURITY ALERT: 3 Failed login attempts reached! Account LOCKED.
[2026-10-05 14:15:27] [WARNING] SECURITY ALERT: Rejected login - Account is LOCKED
