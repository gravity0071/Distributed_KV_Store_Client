#include "ClientToMaster.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h> // 提供 send 和 recv
#include <unistd.h>     // 提供 close

bool ClientToMaster::connectToMaster() {
    // 使用 TcpConnectionUtility 连接到 Master 节点
    socketFd = TcpConnectionUtility::connectToServer("127.0.0.1", 8080);
    if (socketFd < 0) {
        std::cerr << "Failed to connect to Master at 127.0.0.1:8080" << std::endl;
        return false;
    }
//    std::cout << "Connected to Master at 127.0.0.1:8080" << std::endl;
    return true;
}

std::optional<std::string> ClientToMaster::sendRequest(const std::string& key) {
    if (socketFd < 0) {
        std::cerr << "No active connection to Master. Please connect first." << std::endl;
        return std::nullopt;
    }

    // 构造 JSON 查询请求
    std::map<std::string, std::string> requestMap = {
            {"operation", "lookup"},
            {"key", key}
    };
    std::string jsonRequest = jsonParser.MapToJson(requestMap);

    // 发送请求到 Master
    if (send(socketFd, jsonRequest.c_str(), jsonRequest.size(), 0) < 0) {
        perror("Failed to send request to Master");
        return std::nullopt;
    }
    std::cout << "Sent query to Master: " << jsonRequest << std::endl;

    // 接收 Master 的响应
    char buffer[1024] = {0};
    int bytesRead = recv(socketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        perror("Failed to receive response from Master");
        return std::nullopt;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received response from Master: " << buffer << std::endl;

    // 解析响应
    std::map<std::string, std::string> responseMap = jsonParser.JsonToMap(buffer);
    if (responseMap.find("error") != responseMap.end()) {
        std::cerr << "Error from Master: " << responseMap["error"] << std::endl;
        return std::nullopt;
    }

    if (responseMap.find("value") != responseMap.end()) {
        std::cout << "Key: " << responseMap["key"]
                  << ", Store ID: " << responseMap["store_id"]
                  << ", Value: " << responseMap["value"] << std::endl;
        return responseMap["value"];
    }

    std::cerr << "Invalid response format from Master." << std::endl;
    return std::nullopt;
}


void ClientToMaster::closeConnection() {
    if (socketFd >= 0) {
        close(socketFd);
        socketFd = -1;
        std::cout << "Closed connection to Master." << std::endl;
    }
}
