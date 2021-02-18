## Simple Openssl hello world in pure C
The code in this repository starts a simple ssl server. It is written with pure C and use no external libraries or frameworks. The purpose of it is educational only, the code is pretty simple and straightforward without extra modularization or flexibility.

### Usage

 - Clone this repo
 - Run `./run-server.sh` to start two secure and non-secure servers
 - Run `./client.sh` for pinging the non-secure server
 - Run `./secure-client.sh` for pinging the secure server
 - Run `./docker.sh` for running the server inside a Docker container
 - Run `./monitor.sh` for watching over tcp packets to and from those servers

### Tested
- Arch Linux (5.10.16-arch1-1)
