#include "TestServer.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

TestServer::TestServer(const std::string& socketPath)
    : socketPath(socketPath) {}

TestServer::~TestServer() { stop(); }

void TestServer::start() {
  // Check if the socket file exists, and remove it if so
  if (access(socketPath.c_str(), F_OK) == 0) {
      std::cout << "Socket file exists, attempting to remove." << std::endl;
      if (unlink(socketPath.c_str()) == -1) {
          std::cerr << "Failed to remove socket file: " << strerror(errno) << std::endl;
          return;
      }
  }

  running = true;
  serverThread = std::thread(&TestServer::run, this);
}

void TestServer::stop() {
  running = false;
  if (serverThread.joinable()) {
    serverThread.join();
  }
  close(server_fd);
  unlink(socketPath.c_str());
}

void TestServer::run() {
  server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Failed to create socket\n";
    return;
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
  unlink(socketPath.c_str());

  if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    std::cerr << "Bind failed\n";
    return;
  }

  if (listen(server_fd, 5) == -1) {
    std::cerr << "Listen failed\n";
    return;
  }

  while (running) {
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd == -1) {
      if (!running) break;
      std::cerr << "Accept failed\n";
      continue;
    }

    std::thread(&TestServer::handleClient, this, client_fd).detach();
  }
}

void TestServer::handleClient(int client_fd) {
  std::vector<uint8_t> buffer(1024);
  while (running) {
    ssize_t bytesReceived = read(client_fd, buffer.data(), buffer.size());
    if (bytesReceived <= 0) {
      break;
    }

    ssize_t bytesSend = send(client_fd, buffer.data(), bytesReceived, 0);
    if (bytesSend != bytesReceived) {
      std::cerr << "Could not send all bytes" << std::endl;
    }
  }
  close(client_fd);
}
