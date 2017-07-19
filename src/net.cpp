#include <net.h>
#include <iostream>
using namespace std;

Net::Net(char *server, int port) {
    this->server = server;
    this->port = port;
    this->sockAddr = {0};
}

int Net::connect() {
    if (WSAStartup(MAKEWORD(2, 0), &wsaData)!=0) {
        return 0x01;
    }
    if (LOBYTE(wsaData.wVersion) < 2) {
        return 0x02;
    }
    // fill the address
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.S_un.S_addr = findIP(server);
    // create the socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return 0x03;
    }
    if (WINSOCK_API_LINKAGE::connect(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) != 0) {
        return 0x04;
    }
    return 0;
}

int Net::send(const char *data, int bufferSize) {
    if (WINSOCK_API_LINKAGE::send(sock, data, bufferSize-1, 0) == SOCKET_ERROR) {
        return 0x01;
    }
    return 0;
}
int Net::recv(char *buff, int bufferSize) {
    int code;
    code = WINSOCK_API_LINKAGE::recv(sock, buff, bufferSize-1, 0);
    if (code == 0) {
        return 0;
    }
    // manipulate the buffer so we can read it
    // as a null-terminated string
    buff[code] = 0x00;
    return code;
}

void Net::setNonBlocking() {
    unsigned long mode = 1;
    if ((ioctlsocket(sock, FIONBIO, &mode) != 0)) {
        throw "Error setting socket's read block settings.";
    }
}

void Net::close() {
    closesocket(sock);
}

Net::~Net() {

}

unsigned long findIP(const char *server) {
    HOSTENT *host;
    if (!(host = gethostbyname(server))) {
            throw "Could not find the IP address.";
    }
    if (host->h_addr_list && host->h_addr_list[0])
        return *reinterpret_cast<unsigned long*>(host->h_addr_list[0]);

    return 0;
}
