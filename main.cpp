#include "util/ClientToMaster.h"
#include "util/ClientToServer.h"
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

//    std::cout << "Connected to Master successfully." << std::endl;

    // 创建 ClientToServer 实例
    ClientToServer clientToServer(jsonParser);

    // 循环查询键
    std::string key;
    std::string currentServerIp;
    std::string currentServerPort;
    bool connectedToServer = false;

    while (true) {
        std::cout << "Enter the key to lookup (or 'exit' to quit): ";
        std::cin >> key;

        if (key == "exit") {
            std::cout << "Exiting the client." << std::endl;
            break;
        }

        // 向 Master 发送查询请求
        auto masterResponse = clientToMaster.sendRequest(key);
        if (masterResponse.has_value()) {
            const auto& responseMap = masterResponse.value(); // 获取完整的 map
//            std::cout << "Master response map: ";
//            for (const auto& [k, v] : responseMap) {
//                std::cout << k << " = " << v << ", ";
//            }
//            std::cout << std::endl;

            // 提取 IP 和 Port
            if (responseMap.find("ip") != responseMap.end() && responseMap.find("port") != responseMap.end()) {
                std::string serverIp = responseMap.at("ip");
                std::string serverPort = responseMap.at("port");

                // 检查是否需要重新连接到新的服务器
                if (!connectedToServer || serverIp != currentServerIp || serverPort != currentServerPort) {
                    if (connectedToServer) {
                        clientToServer.closeConnection(); // 关闭与当前 Server 的连接
                        connectedToServer = false;
                    }

                    // 尝试连接到新的 Server
                    if (clientToServer.connectToServer(serverIp, serverPort)) {
//                        std::cout << "Connected to Server at " << serverIp << ":" << serverPort << std::endl;
                        currentServerIp = serverIp;
                        currentServerPort = serverPort;
                        connectedToServer = true;
                    } else {
                        std::cerr << "Failed to connect to Server." << std::endl;
                        continue;
                    }
                }

                // 执行操作
                std::string operation;
                std::cout << "Enter operation (read/write/delete): ";
                std::cin >> operation;

                if (operation == "read") {
                    auto serverResponse = clientToServer.sendRequest("read", key);
                    if (serverResponse.has_value()) {
                        std::cout << "Server response: " << serverResponse.value() << std::endl;
                    } else {
                        std::cerr << "Failed to get response from Server." << std::endl;
                    }
                } else if (operation == "write") {
                    std::string value;
                    std::cout << "Enter value to write: ";
                    std::cin >> value;
                    auto serverResponse = clientToServer.sendRequest("write", key, value);
                    if (serverResponse.has_value()) {
                        std::cout << "Write successful. Server response: " << serverResponse.value() << std::endl;
                    } else {
//                        std::cerr << "Failed to write to Server." << std::endl;
                    }
                } else if (operation == "delete") {
                    auto serverResponse = clientToServer.sendRequest("delete", key);
                    if (serverResponse.has_value()) {
                        std::cout << "Delete successful. Server response: " << serverResponse.value() << std::endl;
                    } else {
//                        std::cerr << "Failed to delete key on Server." << std::endl;
                    }
//                } else if (operation == "increment") {
//                    auto serverResponse = clientToServer.sendRequest("increment", key);
//                    if (serverResponse.has_value()) {
//                        std::cout << "Increment successful. New value: " << serverResponse.value() << std::endl;
//                    } else {
//                        std::cerr << "Failed to increment key on Server." << std::endl;
//                    }
                } else {
                    std::cerr << "Unsupported operation: " << operation << std::endl;
                }
            } else {
                std::cerr << "Master did not provide valid Server details." << std::endl;
            }
        } else {
//            std::cerr << "Failed to get response from Master." << std::endl;
        }
    }

    // 关闭与服务器的连接
    if (connectedToServer) {
        clientToServer.closeConnection();
    }

    // 关闭与 Master 的连接
    clientToMaster.closeConnection();
    std::cout << "Connection to Master closed." << std::endl;

    return 0;
}
