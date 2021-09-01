#include "TimeWheel.h"
#include "HttpConn.h"
#include "webServer.h"
#include "Logger.h"

#include <iostream>

TimeWheel::TimeWheel(int size, WebServer *server)
    : size_(size),
      cur_(0),
      wheel_(size, std::list<TcpConnectionPtr>()),
      mtxArr_(size),
      server_(server)
{
}

TimeWheel::~TimeWheel()
{
}

void TimeWheel::insert(const TcpConnectionPtr &newConn)
{
    std::lock_guard<std::mutex> gurad(mtxArr_[cur_]);
    wheel_[cur_].push_back(newConn);
}

void TimeWheel::expired()
{
    for (auto iter = wheel_[cur_].begin(); iter != wheel_[cur_].end();)
    {
        if (!server_->checkAlive(*iter))
        {
            (*iter)->shutdown();
            iter = wheel_[cur_].erase(iter);
            LOG_INFO("KeepAlive::remove Connection!");
        }
        else
        {
            server_->setNotAlive(*iter);
            iter++;
        }
    }
}

void TimeWheel::loop()
{
    expired();
    increament();
}