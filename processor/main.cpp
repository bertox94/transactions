#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
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
bool cleaner_stop = false;
bool process_stop = false;
bool process_stop_set = false;
std::condition_variable cv;
std::condition_variable cv2;
std::mutex mtx; //this guards over both cleaner_stop and thread_pool
std::mutex mtx2; //this guards over process_stop
using namespace std;

void cleaner() {
    cout << "Cleaner initialized\n" << endl;
    while (true) {
        bool flag = false;
        unsigned int n = 0;
        {
            unique_lock<mutex> lk(mtx);
            cv.wait(lk, []() {
                return !thread_pool.empty() || cleaner_stop;
            });
            flag = thread_pool.empty();
            n = thread_pool.size();
        }

        while (!flag) {
            lock_guard<mutex> lk(mtx);
            auto el = thread_pool.begin();
            el->join();
            thread_pool.erase(el);
            flag = thread_pool.empty();
        }

        stringstream aa;
        aa << "------- Cleaned: " << n << " instances." << endl;
        cout << aa.str();
        if (cleaner_stop)
            break;
    }
    cout << "Cleaning complete" << endl;
}

void t_handler(SOCKET ClientSocket) {

    int iResult = 0;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 2) {

        string str = string(recvbuf).substr(0, iResult - 2);
        string resp;

        // if the process should close
        bool close_fg = false;
        if (str.find("close") != string::npos) {
            close_fg = true;
            resp = "OK";
        }

        //report immediately to the main loop whether it should loop again or not.
        {
            lock_guard<mutex> lg(mtx2);
            process_stop = close_fg;
            process_stop_set = true;
        }
        cv2.notify_one();

        // if the process should continue, then choose the right function requested
        if (!process_stop) {
            if (str.find("schedule") != string::npos) {
                str = str.substr(9);
                order o(str);
                resp = schedule(o);
            }
        }

        // Always send back a message
        iSendResult = send(ClientSocket, (resp + "\r\n").c_str(), resp.length() + 2, 0);

        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
        }
    }

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }

    closesocket(ClientSocket);

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

    while (true) {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        cout << "Connection accepted." << endl;
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            break;
        }

        {
            lock_guard<mutex> cleaner_lock(mtx);
            thread_pool.emplace_back(t_handler, ClientSocket);
        }
        cv.notify_one();

        {
            unique_lock<mutex> lk(mtx2);
            cv2.wait(lk, []() {
                return process_stop_set;
            });
            if (process_stop)
                break;
            process_stop_set = false;
        }
    }

    cout << endl;
    cout << "Termination initiated." << endl;
    {
        //make sure to not have thread pool mutex locked here!
        lock_guard<mutex> lockGuard(mtx);
        cleaner_stop = true;
    }
    cv.notify_one();
    t_cleaner.join();

    WSACleanup();
    closesocket(ListenSocket);

    return 0;
}