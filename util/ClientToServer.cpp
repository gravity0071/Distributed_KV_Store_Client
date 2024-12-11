#include "ClientToServer.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <optional>
#include <map>
#include <string>

bool ClientToServer::connectToServer(const std::string& ip, const std::string& port) {
    try {
        int serverPort = std::stoi(port); // Convert port from string to integer
        socketFd = TcpConnectionUtility::connectToServer(ip, serverPort);
        if (socketFd < 0) {
            std::cerr << "[Error] Failed to connect to Server at " << ip << ":" << port << std::endl;
            return false;
        }
//        std::cout << "[Info] Connected to Server at " << ip << ":" << port << " successfully." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Exception] Failed to parse port or connect: " << e.what() << std::endl;
        return false;
    }
}

// 发送请求到服务器并接收响应
std::optional<std::string> ClientToServer::sendRequest(const std::string& operation, const std::string& key, const std::string& value) {
    if (socketFd < 0) {
        std::cerr << "[Error] No active connection to Server. Please connect first." << std::endl;
        return std::nullopt;
    }

    try {
        // 构造 JSON 请求
        std::map<std::string, std::string> requestMap = {
                {"operation", operation},
                {"key", key}
        };

        if (operation == "write") {
            requestMap["value"] = value;
        }

        std::string jsonRequest = jsonParser.MapToJson(requestMap);

        // 向服务器发送请求
        if (send(socketFd, jsonRequest.c_str(), jsonRequest.size(), 0) < 0) {
            perror("[Error] Failed to send request to Server");
            return std::nullopt;
        }
//        std::cout << "[Info] Sent query to Server: " << jsonRequest << std::endl;

        // 接收服务器的响应
        char buffer[1024] = {0};
        int bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cerr << "[Warning] Server closed the connection unexpectedly." << std::endl;
            } else {
                perror("[Error] Failed to receive response from Server");
            }
            closeConnection();
            return std::nullopt;
        }

        buffer[bytesRead] = '\0';
//        std::cout << "[Info] Received response from Server: " << buffer << std::endl;

        // 解析并处理响应
        auto responseMap = jsonParser.JsonToMap(buffer);

        if (responseMap.find("error") != responseMap.end()) {
//            std::cerr << "[Server Error] " << responseMap["error"] << std::endl;
            return std::nullopt;
        }

        if (responseMap.find("value") != responseMap.end()) {
            return responseMap["value"];
        }

//        std::cerr << "[Error] Invalid response format from Server." << std::endl;
        return std::nullopt;

    } catch (const std::exception& e) {
//        std::cerr << "[Exception] Error during request: " << e.what() << std::endl;
        return std::nullopt;
    }
}

// 关闭与服务器的连接
void ClientToServer::closeConnection() {
    if (socketFd >= 0) {
        close(socketFd);
        socketFd = -1;
//        std::cout << "[Info] Closed connection to Server." << std::endl;
    } else {
        std::cerr << "[Warning] Connection to Server already closed." << std::endl;
    }
}

void ClientToServer::run() {
    std::string ip = "127.0.0.1";
    std::string port = "8080";

    if (!connectToServer(ip, port)) {
//        std::cerr << "[Error] Unable to establish connection to the server." << std::endl;
        return;
    }

    std::string operation, key, value;

    while (true) {
        std::cout << "\nEnter operation (read/write/delete/increment/exit): ";
        std::cin >> operation;

        if (operation == "exit") {
            break;
        }

        std::cout << "Enter key: ";
        std::cin >> key;

        if (operation == "write") {
            std::cout << "Enter value: ";
            std::cin >> value;
        } else {
            value = "";
        }

        auto response = sendRequest(operation, key, value);

        if (response) {
            std::cout << "[Response] " << *response << std::endl;
        } else {
            std::cout << "[Error] Failed to get a response from the server." << std::endl;
        }
    }

    closeConnection();
}
