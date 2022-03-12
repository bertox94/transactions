#undef UNICODE

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "scheduler.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

std::list<thread> thread_pool;
std::condition_variable cv;
std::mutex cleaner_mutex;

[[noreturn]] void cleaner() {
    bool flagg = true;
    while (flagg) {
        cout << "P" << endl;
        {
            unique_lock<mutex> cleaner_lock(cleaner_mutex);
            cv.wait(cleaner_lock, []() {
                return !thread_pool.empty();
            });
        }

        bool flag = false;
        do {
            unique_lock<mutex> cleaner_lock(cleaner_mutex);
            auto el = thread_pool.begin();
            el->join();
            thread_pool.erase(el);
            flag = thread_pool.empty();
        } while (!flag);
    }
    return;
}

int initialize(SOCKET &ListenSocket) {

    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

void t_handler(SOCKET ClientSocket) {

    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];

    int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    printf("Bytes received: %d\n", iResult);

    string str = string(recvbuf).substr(0, iResult - 2);

    if (str.find("schedule") != string::npos) {
        str = str.substr(9);
        order o(str);
        str = schedule(o);
    }

    iSendResult = send(ClientSocket, (str + "\r\n").c_str(), iResult + 2, 0);
    printf("Bytes sent: %d\n", iSendResult);
    closesocket(ClientSocket);
    WSACleanup();

}

int __cdecl main(void) {
    thread cleaner_thread(cleaner);

    auto ListenSocket = INVALID_SOCKET;
    auto ClientSocket = INVALID_SOCKET;

    initialize(ListenSocket);

    while (true) {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        //if (ClientSocket == INVALID_SOCKET) {
        //    printf("accept failed with error: %d\n", WSAGetLastError());
        //    closesocket(ListenSocket);
        //    WSACleanup();
        //    return 1;
        //}

        int iSendResult;
        int recvbuflen = DEFAULT_BUFLEN;
        char recvbuf[DEFAULT_BUFLEN];

        int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        printf("Bytes received: %d\n", iResult);

        string str = string(recvbuf).substr(0, iResult - 2);

        if (str.find("schedule") != string::npos) {
            str = str.substr(9);
            order o(str);
            str = schedule(o);
        }

        iSendResult = send(ClientSocket, (str + "\r\n").c_str(), iResult + 2, 0);
        printf("Bytes sent: %d\n", iSendResult);

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        printf("Bytes received: %d\n", iResult);


        closesocket(ClientSocket);
        WSACleanup();

    }


    closesocket(ListenSocket);
    cleaner_thread.join();

    return 0;
}