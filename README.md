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

```bash
sudo apt update
sudo apt install build-essential libssl-dev openssl -y

