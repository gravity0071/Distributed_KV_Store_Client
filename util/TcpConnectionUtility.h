#ifndef TCPCONNECTIONUTILITY_H
#define TCPCONNECTIONUTILITY_H

#include <string>

class TcpConnectionUtility {
public:
    // 创建与服务器的 TCP 连接
    static int connectToServer(const std::string& serverIp, int serverPort, int localPort = 0);
};

#endif // TCPCONNECTIONUTILITY_H
