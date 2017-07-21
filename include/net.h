#ifndef NET_H_INCLUDED
#define NET_H_INCLUDED

#define WIN32_MEAN_AND_LEAN
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>

class Net {
private:
    WSAData wsaData;
    SOCKET sock;
    sockaddr_in sockAddr;

    char *server;
    int port;

public:
    // constructor
    Net(char *server, int port);
    // destructor
    ~Net();
    // connect to the remote server
    int connect();
    // send data to the remote server
    int send(const char *data, int bufferSize);
    // receive data from the remote server
    int recv(char *buff, int bufferSize);
    // close the connection
    void close();
    // set whether the receive is blocking or not
    void setNonBlocking();
    // set the timeout of the receiving function
    void setTimeout(int time);


};

unsigned long findIP(const char *server);

#endif // NET_H_INCLUDED
