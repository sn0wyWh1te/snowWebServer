#include "EventLoop.h"

#include <iostream>
#include <functional>

class Printer
{
public:
    Printer(EventLoop *loop)
    :loop_(loop)
    ,cnt_(0)
    {
    }

    void print()
    {
        std::cout << "cnt is " << cnt_ << std::endl;
        cnt_++;
    }

    EventLoop *loop_;
    int cnt_;
};

int main()
{
    EventLoop loop;
    Printer printer(&loop);
    loop.runEvery(5, std::bind(&Printer::print, &printer));
    loop.loop();
    return 0;
}