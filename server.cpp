#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    std::cout << "Server running on port 8080..." << std::endl;

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        std::cout << "Client connected!" << std::endl;

        const char* msg =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 24\r\n"
            "\r\n"
            "Hello from C++ server!";

        send(client_socket, msg, strlen(msg), 0);
        close(client_socket);
    }

    close(server_fd);
    return 0;
}
