#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "threadpool.h"

#define MAX_EVENTS 1024

// 处理客户端请求
void handleClient(int fd) {
    char buffer[1024] = {0};
    int bytes_read = read(fd, buffer, sizeof(buffer));

    if (bytes_read <= 0) {
        close(fd);
        return;
    }

    // ===== 日志：原始请求 =====
    std::cout << "========== New Request ==========" << std::endl;
    std::cout << buffer << std::endl;

    std::string request(buffer);

    // ===== 解析路径 =====
    std::string path = "/";
    size_t pos1 = request.find(" ");
    size_t pos2 = request.find(" ", pos1 + 1);

    if (pos1 != std::string::npos && pos2 != std::string::npos) {
        path = request.substr(pos1 + 1, pos2 - pos1 - 1);
    }

    // ===== 日志：解析结果 =====
    std::cout << "Parsed Path: " << path << std::endl;

    // ===== 路由逻辑 =====
    std::string body;

    if (path == "/hello") {
        body = "Hello";
    } else if (path == "/test") {
        body = "Test OK";
    } else {
        body = "Home";
    }

    // ===== 构造 HTTP 响应 =====
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;

    send(fd, response.c_str(), response.size(), 0);

    close(fd);
}

int main() {
    // ===== 创建 socket =====
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 端口复用（避免重启报错）
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 128);

    // ===== 创建 epoll =====
    int epoll_fd = epoll_create1(0);

    epoll_event ev{}, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    // ===== 创建线程池 =====
    ThreadPool pool(4);

    std::cout << "🚀 ThreadPool Web Server running on port 8080..." << std::endl;

    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            // ===== 新连接 =====
            if (fd == server_fd) {
                int client_fd = accept(server_fd, nullptr, nullptr);

                epoll_event client_ev{};
                client_ev.events = EPOLLIN;
                client_ev.data.fd = client_fd;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

                std::cout << "New client connected: fd=" << client_fd << std::endl;
            }
            // ===== 已连接客户端 =====
            else {
                int client_fd = fd;

                pool.enqueue([client_fd]() {
                    handleClient(client_fd);
                });
            }
        }
    }

    close(server_fd);
    return 0;
}
