# Project1-Middleware MOM

This project implements a **Message Oriented Middleware** (MOM) using **gRPC** and **Protobuf** in C++. The system allows the creation, deletion and management of **topics** and **queues** in a distributed environment. In addition, it implements an authentication and registration system for clients.

# Requirements 

This project uses several dependencies that you must install before you can compile and run the code.

## Necessary dependencies

1. **gRPC** and **Protobuf**  
   - These are the main libraries for communication between services using gRPC and for message serialization with Protobuf.
   
2. **vcpkg**  
   - We use `vcpkg` to manage library dependencies.

3. **OpenSSL**  
   - For cryptography and JWT handling (for authentication).

4. **libpqxx**  
   - Client library for working with PostgreSQL databases.

## Dependencies instalation

### Install vcpkg (if you don't have it)

vcpkg is a free and open source C/C++ package manager, maintained by Microsoft and the C++ community. It facilitates the management of libraries in C/C++ projects, allowing you to easily install, update and remove dependencies.
If you do not have `vcpkg` installed, follow these steps to install it:


Clone the vcpkg repository

````bash
git clone https://github.com/Microsoft/vcpkg.git
````

Access the folder

````bash
cd vcpkg
````

Run the initialization script

````bash
./bootstrap-vcpkg.sh
````

Add vcpkg to the PATH (optional, for global ease of use)

````bash
export PATH=$PWD/vcpkg:$PATH
````

### Install the necessary dependencies with vcpkg

We use vcpkg to install the necessary dependencies for the project. Make sure vcpkg is correctly installed and then run the following comands:

To install protobuf:
````bash
./vcpkg install protobuf grpc libpqxx openssl
````

To install grpc:
````bash
./vcpkg intall grpc
````

To install libpqxx:
````bash
./vcpkg install libpqxx
````

To install openssl:
````bash
./vcpkg install openssl
````

### Install CMake

If you do not have CMake installed, install it with the following command (on Linux):
````bash
sudo apt-get install cmake
````
On Windows, you can download it from CMake's official website.

## Project Compilation

### Enviroment configuration

Before compiling, make sure that the vcpkg environment variables are set correctly. If you have not already done so, add vcpkg to the PATH:
````bash
# Para Linux y macOS
export PATH=$HOME/vcpkg/installed/x64-linux/tools:$PATH

# Para Windows (en PowerShell)
$env:Path += ";C:\path\to\vcpkg\installed\x64-windows\tools"
````

### Generate Protobuf files
The project includes .proto files that must be processed to generate the corresponding C++ classes.
You can use the build_all.sh script to generate the necessary files from the .proto files.
````bash
./build_all.sh
````
This script will compile all .proto files into the corresponding .pb.h and .grpc.pb.h files, and place them in the build/generated folder.

### Compile the project
Once you have generated the necessary Protobuf files, you can compile the project with CMake:

- Enter the compilation directory:
````bash
cd build
````

- Execute CMake:
````bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
````

- Compile the project:
````bash
make -j
````
This command will compile the project and generate the executable in the build folder.


## Execute the project

Before running the project, follow these steps:

### Create the environment configuration file

You must create a file named `src/config.env` containing the environment variables required for the database connection and JWT secret. Here is an example:

This file provides the database connection string and JWT secret - essential for the binary to authenticate with PostgreSQL and secure API endpoints.
```env
# File: src/config.env
JWT_SECRET=ncodinvpdinpqo84h3n0
DB_CONNECTION=postgresql://ubuntu:+Juanz2314@localhost:5432/testdatabase
```
### Install PostgreSQL:
PostgreSQL installation provides the database backend required for the MOM system to store user credentials and queue metadata.
```bash
sudo apt update && sudo apt install -y postgresql libpqxx-dev libpq-dev
```

### Create PostgreSQL user and database:
Creates a privileged database user and schema to allow the binary to manage authentication and persist system data through PostgreSQL.
````bash

sudo -u postgres psql -c "CREATE USER ubuntu WITH PASSWORD '+Juanz2314';"

sudo -u postgres psql -c "ALTER USER ubuntu WITH SUPERUSER CREATEDB CREATEROLE;"

sudo -u postgres psql -c "CREATE DATABASE testDatabase OWNER ubuntu;"

sudo -u postgres psql -d testDatabase -c "CREATE TABLE users (id SERIAL PRIMARY KEY, username VARCHAR(50) UNIQUE NOT NULL, password_hash TEXT NOT NULL);"
````

### Configure PostgreSQL authentication:
Modifies authentication from peer-to-password method, enabling the binary to connect using the provided credentials in DB_CONNECTION.
````bash
sudo -sed -i 's/^local\s\+all\s\+all\s\+peer/local all all md5/' /etc/postgresql/*/main/pg_hba.conf
sudo systemctl restart postgresql
````

### Running the server 
Execute the compiled binary with the required parameters. Basic usage:
````bash
./mom <grpc_port> [rest_port] [peer1] [peer2] ... [--host <port>] [--announce <host_ip>]
````
The binary connects to PostgreSQL using the configured credentials, while parameters define network interfaces and cluster peers for distributed operations.
Examples:
````bash
./mom 50051 8080 peer1:50051 peer2:50052 --host 6000 --announce 192.168.1.100
````
## Explore Project Resources

📖 **Project Documentation**  
Visit our [Project Wiki](https://github.com/AnaSofiR/Project1-Middleware/wiki) for detailed guides, architecture diagrams, and usage examples.  
*"Understand system design decisions and implementation specifics through our comprehensive documentation."*

🗂️ **Development Progress**  
Track features and priorities in our [Project Backlog](https://github.com/users/AnaSofiR/projects/10).  
*"See what's being worked on, upcoming enhancements, and contribute to feature prioritization."*
