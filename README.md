# C++ Custom HTTP Server

A high-performance, custom-built HTTP server written entirely in C++ from scratch. This project implements its own HTTP parsing, routing, and request/response handling without relying on external web frameworks. 

## Features

- **Custom HTTP & Multipart Parsing**: Built-in HTTP request parsing with support for query parameters, headers, cookies, and `multipart/form-data` file uploads.
- **Thread Pool Architecture**: Efficient request handling using a custom thread pool.
- **PostgreSQL Connection Pool**: A robust, thread-safe database connection pool for handling multiple concurrent queries safely.
- **Authentication System**: Secure user registration, login, and session management using tokens and cookies.
- **File Management**: Upload, download, compress, and delete files securely.
- **Action History**: Tracks user actions automatically in the database.

## Prerequisites

- **WSL (Ubuntu/Debian)** or Native Linux environment
- **CMake** (v3.10+)
- **C++20** compatible compiler (GCC/Clang)
- **PostgreSQL** server running locally (`username: postgres`)
- **libpq-dev** installed (`sudo apt-get install libpq-dev`)

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
