# C++ High-Performance Web Server

## 项目介绍
基于 C++ 实现的高性能 Web 服务器，采用 epoll + 线程池模型，支持高并发连接处理。

## 技术栈
- C++17
- Linux Socket API
- epoll
- 多线程（thread pool）

## 核心功能
- 基于 epoll 的 IO 多路复用
- 线程池并发处理请求
- 支持 HTTP 基本响应
- 支持多连接并发访问

## 架构设计
- 主线程：负责 epoll 事件监听
- 工作线程：线程池处理客户端请求
- Reactor 模型（简化版）

## 如何运行

```bash
mkdir build
cd build
cmake ..
make
./server
