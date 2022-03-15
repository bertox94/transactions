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
#define DEFAULT_PORT "27017"

std::list<shared_ptr<thread>> thread_pool;
bool cleaner_stop = false;
bool process_stop = false;
bool process_stop_set = false;
std::condition_variable cv;
std::condition_variable cv2;
std::mutex mtx; //this guards over both cleaner_stop and thread_pool
std::mutex mtx2; //this guards over process_stop
std::mutex mtx3; //this guards over cout
using namespace std;

void cleaner() {
    cout << "Cleaner is operational.\n" << endl;
    bool local = false;
    while (true) {
        unsigned int n = 0;
        {
            unique_lock<mutex> LOCK(mtx);
            cv.wait(LOCK, []() {
                return !thread_pool.empty() || cleaner_stop;
            });
            local = cleaner_stop;
        }

        while (true) {
            _List_iterator<_List_val<_List_simple_types<shared_ptr<thread>>>> el;
            {
                lock_guard<mutex> GUARD(mtx);
                if (thread_pool.empty())
                    break;
                el = thread_pool.begin();
            }
            (*el)->join();
            n++;
            {
                lock_guard<mutex> GUARD(mtx);
                thread_pool.erase(el);
            }
        }
        {
            lock_guard<mutex> GUARD(mtx3);
            cout << "------- Cleaned: " << n << (n == 1 ? " instance." : " instances.") << endl;
        }

        if (local)
            break;
    }
    {
        lock_guard<mutex> GUARD(mtx3);
        cout << "Cleaning complete." << endl;
    }
}

void t_handler(SOCKET ClientSocket) {

    int iResult = 0;
    int iSendResult = 0;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 2) {

        string str = string(recvbuf).substr(0, iResult - 2);
        string resp;

        // check if the process should close ...
        bool local = false;
        if (str.find("close") != string::npos) {
            {
                lock_guard<mutex> lg(mtx3);
                cout << "\nTermination initiated." << endl;
            }
            local = true;
            resp = "OK";
        }

        //... and report immediately to the main loop whether it should loop again or not, so that it can accept new connections
        {
            lock_guard<mutex> guard(mtx2);
            process_stop = local;
            process_stop_set = true;
        }
        cv2.notify_one();

        // if the process should continue, then choose the function requested
        if (!local) {
            string cmd = str.substr(0, str.find('\n'));
            if (cmd == "schedule") {
                str = str.substr(str.find('\n') + 1);
                order o(str);
                resp = schedule(o);
            } else if (cmd == "quickcheck") {

            }
        }

        // Always send back a message
        iSendResult = send(ClientSocket, (resp + "\r\n").c_str(), resp.length() + 2, 0);

        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
        }

        {
            lock_guard<mutex> lg(mtx3);
            cout << "Request executed." << endl;
        }
    }

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
    }

    closesocket(ClientSocket);
}

int initializeSSocket(SOCKET &ListenSocket) {

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

int __cdecl main() {

    auto ListenSocket = INVALID_SOCKET;
    if (!initializeSSocket(ListenSocket)) {
        thread t_cleaner(cleaner);
        auto ClientSocket = INVALID_SOCKET;
        while (true) {
            ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                printf("accept failed with error: %d\n", WSAGetLastError());
                break;
            }

            auto tp = make_shared<thread>(t_handler, ClientSocket);
            {
                lock_guard<mutex> GUARD(mtx);
                thread_pool.push_back(tp);
            }
            cv.notify_one();

            {
                unique_lock<mutex> LOCK(mtx2);
                cv2.wait(LOCK, []() {
                    return process_stop_set;
                });
                if (process_stop)
                    break;
                process_stop_set = false;
            }
        }

        {
            lock_guard<mutex> GUARD(mtx);
            cleaner_stop = true;
        }
        cv.notify_one();
        t_cleaner.join();

        WSACleanup();
        closesocket(ListenSocket);
    }

    return 0;
}