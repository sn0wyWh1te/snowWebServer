#pragma once

#include "noncopyable.h"
#include "webServer.h"

class WebService : noncopyable
{
public:
    WebService(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg,double timeout);
    ~WebService() = default;
    void start() { server_.start(); };

private:
    std::string doRequest(HttpConn *conn);

private:
    WebServer server_;
};
