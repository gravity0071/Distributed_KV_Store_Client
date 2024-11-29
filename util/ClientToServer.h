//
// Created by Sky on 2024/11/19.
//

#ifndef CLIENT_TO_SERVER_H
#define CLIENT_TO_SERVER_H

#include "JsonParser.h"
#include "TcpConnectionUtility.h"
#include <string>
#include <optional>

class ClientToServer {
private:
    int socketFd;                 // Server 连接的 Socket 文件描述符
    JsonParser& jsonParser;       // 用于解析和构造 JSON 数据

public:
    explicit ClientToServer(JsonParser& jsonParser)
            : socketFd(-1), jsonParser(jsonParser) {}

    ~ClientToServer() {
        closeConnection();
    }

    // 与指定的 Server 连接
    bool connectToServer(const std::string& ip, const std::string& port);

    // 向 Server 发送查询请求
    std::optional<std::string> sendRequest(const std::string& operation, const std::string& key, const std::string& value = "");

    // 关闭与 Server 的连接
    void closeConnection();

    // 持续运行客户端的交互逻辑
    void run();
};

#endif // CLIENT_TO_SERVER_H
