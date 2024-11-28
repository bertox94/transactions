#include <stdlib.h>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>

using namespace std;

std::list<thread *> thread_pool;
std::condition_variable cv;
std::mutex cleaner_mutex;


void cleaner() {
    bool flagg = true;
    while (flagg) {
        cout << "P" << endl;
        {
            unique_lock<mutex> cleaner_lock(cleaner_mutex);
            cv.wait(cleaner_lock, []() {
                return !thread_pool.empty();
            });
        }

        thread_pool.clear();
        unique_lock<mutex> cleaner_lock(cleaner_mutex);
        cv.wait(cleaner_lock, []() {
            return !thread_pool.empty();
        });

        bool flag = false;
        do {
            unique_lock<mutex> cleaner_lock(cleaner_mutex);
            auto el = thread_pool.begin();
            auto ptr = *el;
            thread_pool.erase(el);
            flag = thread_pool.empty();
            ptr->join();
        } while (!flag);
    }
    return;
}

void func() {
    cout << "Hi" << endl;
}


int main(void) {
    std::thread cc(cleaner);

    for (int i = 0; i < 200000; i++) {};

    std::thread t(func);
    {
        std::lock_guard<std::mutex> lk(cleaner_mutex);
        thread_pool.push_back(&t);
    }
    cout << "K" << endl;
    cv.notify_all();
    cc.join();

    return 0;
}