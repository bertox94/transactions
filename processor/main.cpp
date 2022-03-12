#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <list>
#include <condition_variable>
#include <mutex>
#include "scheduler.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

std::list<std::thread> thread_pool;
std::condition_variable cv;
std::mutex tHread_pool_mutex;
std::mutex cleaner_mutex;
bool cleaner_stop = false;

using namespace std;

void cleaner() {
    cout << "Cleaner initialized" << endl;
    while (true) {
        {
            unique_lock<mutex> thread_pool_lock(tHread_pool_mutex);


            cv.wait(thread_pool_lock, []() {
                return !thread_pool.empty() ;
            });
        }
        {
            lock_guard<mutex> cleaner_guard(cleaner_mutex);
            if (cleaner_stop)
                break;
        }

        bool flag = false;
        do {
            unique_lock<mutex> cleaner_lock(tHread_pool_mutex);
            auto el = thread_pool.begin();
            el->join();
            thread_pool.erase(el);
            flag = thread_pool.empty();
        } while (!flag);
    }
}

void t_handler(SOCKET ClientSocket) {

    int iResult = 0;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 2) {
            printf("Bytes received: %d\n", iResult);

            string str = string(recvbuf).substr(0, iResult - 2);

            if (str.find("schedule") != string::npos) {
                str = str.substr(9);
                order o(str);
                str = schedule(o);
            }

            iSendResult = send(ClientSocket, (str + "\r\n").c_str(), iResult + 2, 0);

            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 2) {
            printf("Connection closing...\n");
            cout << endl;
            // shutdown the connection since we're done
            iResult = shutdown(ClientSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
            }

            // cleanup
            closesocket(ClientSocket);
        } else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
        }

    } while (iResult > 0);

}

int __cdecl main() {

    thread t_cleaner(cleaner);

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

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

    bool flag = true;
    while (flag) {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        {
            lock_guard<mutex> cleaner_lock(tHread_pool_mutex);
            thread_pool.emplace_back(t_handler, ClientSocket);
        }
        cv.notify_one();

    }

    WSACleanup();
    closesocket(ListenSocket);
    {
        lock_guard<mutex> lockGuard(cleaner_mutex);
        cleaner_stop = true;
    }
    t_cleaner.join();

    return 0;
}