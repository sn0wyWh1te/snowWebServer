#pragma once

#include <vector>
#include <list>
#include <functional>
#include <mutex>

#include "noncopyable.h"
#include "Callbacks.h"

class HttpConn;
class WebServer;

class TimeWheel : noncopyable
{
public:
    TimeWheel(int size, WebServer *server);
    ~TimeWheel();
    void insert(const TcpConnectionPtr &newConn);
    void loop();

private:
    void expired();
    void increament()
    {
        ++cur_;
        cur_ %= size_;
    }

private:
    int cur_;  //current index
    int size_; //size of wheel
    std::vector<std::list<TcpConnectionPtr>> wheel_;
    std::vector<std::mutex> mtxArr_;
    WebServer *server_;
};
