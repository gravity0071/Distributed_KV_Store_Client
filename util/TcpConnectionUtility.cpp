#include "TcpConnectionUtility.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h> // 用于 socket 操作
#include <unistd.h>    // 用于 close()

int TcpConnectionUtility::connectToServer(const std::string& serverIp, int serverPort, int localPort) {
    // 创建 socket
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // 如果指定了本地端口，则绑定
    if (localPort > 0) {
        sockaddr_in localAddress{};
        localAddress.sin_family = AF_INET;
        localAddress.sin_addr.s_addr = INADDR_ANY; // 绑定到所有本地接口
        localAddress.sin_port = htons(localPort);

        if (bind(socketFd, (struct sockaddr*)&localAddress, sizeof(localAddress)) < 0) {
            perror("Binding to local port failed");
            close(socketFd);
            return -1;
        }

        std::cout << "Bound to local port " << localPort << std::endl;
    }

    // 设置服务器地址
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // 转换 IP 地址
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(socketFd);
        return -1;
    }

    // 连接服务器
    if (connect(socketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection to server failed");
        close(socketFd);
        return -1;
    }

//    std::cout << "Connected to server at " << serverIp << ":" << serverPort << std::endl;
    return socketFd;
}
