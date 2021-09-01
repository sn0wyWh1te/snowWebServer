#pragma once

#include "TcpServer.h"
#include "EventLoop.h"
#include "HttpConn.h"
#include "TimeWheel.h"

class WebServer : noncopyable
{
public:
    using httpCallBack = std::function<std::string(HttpConn *)>;
    WebServer(EventLoop *loop,
              const InetAddress &listenAddr,
              const std::string &nameArg,
              double timeout);

    void start();
    void setHttpCallBack(const httpCallBack &cb) { cb_ = cb; };
    bool checkAlive(const TcpConnectionPtr &conn)
    {
        if (httpConnMap_.count(conn))
        {
            return httpConnMap_[conn].checkAlive();
        }
        return false;
    }

    void setNotAlive(const TcpConnectionPtr &conn)
    {
        if (httpConnMap_.count(conn))
        {
            return httpConnMap_[conn].setAlive(false);
        }
    }

private:
    void onConnection(const TcpConnectionPtr &);
    void onMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp);

    TcpServer _server;
    EventLoop *_loop;
    httpCallBack cb_;
    TimeWheel wheel_;
    std::unordered_map<TcpConnectionPtr, HttpConn> httpConnMap_;
    std::mutex mtx_;
};
