#ifndef CLIENT_TO_MASTER_H
#define CLIENT_TO_MASTER_H

#include "TcpConnectionUtility.h"
#include "JsonParser.h"
#include <string>
#include <optional>
#include <map> // 新增：用于 std::map 类型

class ClientToMaster {
private:
    JsonParser& jsonParser; // JSON 解析器
    int socketFd;           // 与 Master 的连接套接字

public:
    // 构造函数，初始化 JSON 解析器
    explicit ClientToMaster(JsonParser& jsonParser)
            : jsonParser(jsonParser), socketFd(-1) {}

    // 连接到 Master 节点
    bool connectToMaster();

    // 向 Master 发送查询请求并接收响应
    std::optional<std::map<std::string, std::string>> sendRequest(const std::string& key);

    // 关闭与 Master 的连接
    void closeConnection();
};

#endif // CLIENT_TO_MASTER_H
