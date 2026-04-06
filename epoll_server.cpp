#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define MAX_EVENTS 10

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    int epoll_fd = epoll_create1(0);

    epoll_event ev{}, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "Epoll server running on port 8080..." << std::endl;

    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);

                epoll_event client_ev{};
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_fd;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

                std::cout << "New client connected" << std::endl;
            } else {
                char buffer[1024] = {0};
                read(fd, buffer, sizeof(buffer));

                const char* response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 24\r\n"
                    "\r\n"
                    "Hello from epoll server!";

                send(fd, response, strlen(response), 0);

                close(fd);
            }
        }
    }

    close(server_fd);
    return 0;
}
