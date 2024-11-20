//
// Created by Sky on 2024/11/19.
//
#include "ClientToServer.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h> // 提供 send 和 recv
#include <unistd.h>     // 提供 close

bool ClientToServer::connectToServer(const std::string& ip, const std::string& port) {
    int serverPort = std::stoi(port); // 将端口从字符串转换为整数
    socketFd = TcpConnectionUtility::connectToServer(ip, serverPort);
    if (socketFd < 0) {
        std::cerr << "Failed to connect to Server at " << ip << ":" << port << std::endl;
        return false;
    }
    std::cout << "Connected to Server at " << ip << ":" << port << " successfully." << std::endl;
    return true;
}

std::optional<std::string> ClientToServer::sendRequest(const std::string& operation, const std::string& key, const std::string& value) {
    if (socketFd < 0) {
        std::cerr << "No active connection to Server. Please connect first." << std::endl;
        return std::nullopt;
    }

    // 构造 JSON 请求
    std::map<std::string, std::string> requestMap = {
            {"operation", operation},
            {"key", key}
    };

    // 如果是写操作，附加 value 字段
    if (operation == "write") {
        requestMap["value"] = value;
    }

    std::string jsonRequest = jsonParser.MapToJson(requestMap);

    // 发送请求到 Server
    if (send(socketFd, jsonRequest.c_str(), jsonRequest.size(), 0) < 0) {
        perror("Failed to send request to Server");
        return std::nullopt;
    }
    std::cout << "Sent query to Server: " << jsonRequest << std::endl;

    // 接收 Server 的响应
    char buffer[1024] = {0};
    int bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        perror("Failed to receive response from Server");
        return std::nullopt;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received response from Server: " << buffer << std::endl;

    // 解析响应
    std::map<std::string, std::string> responseMap = jsonParser.JsonToMap(buffer);

    // 检查响应中的错误
    if (responseMap.find("error") != responseMap.end()) {
        std::cerr << "Error from Server: " << responseMap["error"] << std::endl;
        return std::nullopt;
    }

    // 返回响应中的值（如果存在）
    if (responseMap.find("value") != responseMap.end()) {
        return responseMap["value"];
    }

    return std::nullopt;
}

void ClientToServer::closeConnection() {
    if (socketFd >= 0) {
        close(socketFd);
        socketFd = -1;
        std::cout << "Closed connection to Server." << std::endl;
    }
}
