# Multithreaded HTTP File Transfer Server

A high-performance, custom-built HTTP server written entirely in C++17 from scratch. This project implements its own HTTP parsing, routing, and request/response handling without relying on external web frameworks. 

## Features

- Developed a high-performance network file server utilizing C++17 and native POSIX Sockets for concurrent client request operations.
- Integrated a memory-optimized HTTP parser configured for multipart/form-data to stabilize binary payloads during heavy file processing.
- Secured user endpoints by deploying OpenSSL cryptographic validations to mitigate unauthorized route access exploits.
- Constructed a download routing system mapped to access privileges, managed by a PostgreSQL persistent database backend.

## Prerequisites

- **WSL (Ubuntu/Debian)** or Native Linux environment
- **CMake** (v3.10+)
- **C++17** compatible compiler (GCC/Clang)
- **PostgreSQL** server running locally (`username: postgres`)
- **libpq-dev** installed (`sudo apt-get install libpq-dev`)
- **libssl-dev** installed (`sudo apt-get install libssl-dev`)

## Database Setup

Ensure you have a PostgreSQL user named `postgres` configured, and run the following SQL schema to initialize the database tables:

```sql
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE history (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id) ON DELETE CASCADE,
    action TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Note: The implementation also expects `user_sessions` and `files` tables as seen in database queries.
```

## How to Build

1. Open your WSL or Linux terminal.
2. Navigate to the project root directory.
3. Configure the project using CMake:
   ```bash
   cmake -B build -S .
   ```
4. Build the executable:
   ```bash
   cmake --build build
   ```

## How to Run

Once compiled, you can start the server by running the executable generated in the `build` directory, passing the desired port number as an argument:

```bash
./build/HttpServer <port>
```
*Example:* `./build/HttpServer 8080`

The server will initialize the connection pool, register all defined routes, and start listening for incoming HTTP connections on the specified port.
