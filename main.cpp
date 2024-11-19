#include "util/ClientToMaster.h"
#include "util/JsonParser.h"
#include <iostream>

int main() {
    // 初始化 JSON 解析器
    JsonParser jsonParser;

    // 创建 ClientToMaster 实例
    ClientToMaster clientToMaster(jsonParser);

    // 连接到 Master 节点
    if (!clientToMaster.connectToMaster()) {
        std::cerr << "Failed to connect to Master. Exiting." << std::endl;
        return -1;
    }

    std::cout << "Connected to Master successfully." << std::endl;

    // 循环查询键
    std::string key;
    while (true) {
        std::cout << "Enter the key to lookup (or 'exit' to quit): ";
        std::cin >> key;

        if (key == "exit") {
            std::cout << "Exiting the client." << std::endl;
            break;
        }

        // 向 Master 发送查询请求
        auto response = clientToMaster.sendRequest(key);
        if (response.has_value()) {
            std::cout << "Master response: " << response.value() << std::endl;
        } else {
            std::cerr << "Failed to get response from Master." << std::endl;
        }
    }

    // 关闭连接
    clientToMaster.closeConnection();
    std::cout << "Connection to Master closed." << std::endl;

    return 0;
}
