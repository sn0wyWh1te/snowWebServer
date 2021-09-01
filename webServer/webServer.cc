#include "webServer.h"
#include "HttpConn.h"
#include "Logger.h"

#include <iostream>
#include <functional>
#include <string>
#include <memory>

using namespace std;
using namespace placeholders;

WebServer::WebServer(EventLoop *loop,
                     const InetAddress &listenAddr,
                     const string &nameArg,
                     double timeout)
    : _server(loop, listenAddr, nameArg), _loop(loop),
      wheel_(5, this)
{
    _server.setConnectionCallback(std::bind(&WebServer::onConnection, this, _1));
    _server.setMessageCallback(std::bind(&WebServer::onMessage, this, _1, _2, _3));
    _server.setThreadNum(3);
    _loop->runEvery(timeout, std::bind(&TimeWheel::loop, &wheel_));
}

void WebServer::start()
{
    _server.start();
}

void WebServer::onConnection(const TcpConnectionPtr &conn)
/*
if a connection is hunged up,close the the connection 
and remove it from connection map in TcpServer
*/
{
    if (!conn->connected())
    {
        std::lock_guard<std::mutex> guard(mtx_);
        httpConnMap_.erase(conn);
    }
    LOG_INFO("Connection shutdown!");
}

void WebServer::onMessage(const TcpConnectionPtr &conn,
                          Buffer *buffer,
                          Timestamp time)
{
    bool firstFlag = false;
    HttpConn *httpConnPtr = nullptr;
    {
        std::lock_guard<std::mutex> guard(mtx_);
        auto iter = httpConnMap_.find(conn);
        if (iter == httpConnMap_.end())
        {
            firstFlag = true;
            httpConnMap_.emplace(std::piecewise_construct, std::forward_as_tuple(conn), std::forward_as_tuple());
            httpConnMap_[conn].init(cb_);
        }
        httpConnPtr = &httpConnMap_[conn];
    }

    //FIX ME:use c++17 string_view to avoid copy
    std::string msg = buffer->peekAllAsString();
    HttpConn::HTTP_CODE code = httpConnPtr->process(&msg);
    if (code == HttpConn::NO_REQUEST)
    {
        if (firstFlag == true)
        {
            wheel_.insert(conn);
            return;
        }
        httpConnPtr->setAlive(true);
        return;
    }
    else if (code == HttpConn::GET_REQUEST)
    {
        conn->send(httpConnPtr->getResponseHead());
        conn->sendFile(httpConnPtr->getFilePath().c_str());
        httpConnPtr->init(cb_);
        buffer->retrieveAll();

        if (firstFlag == true)
        {
            wheel_.insert(conn);
        }
    }
    else
    {
        if (firstFlag == false)
        {
            return;
        }
        conn->shutdown();
    }
    return;
}
