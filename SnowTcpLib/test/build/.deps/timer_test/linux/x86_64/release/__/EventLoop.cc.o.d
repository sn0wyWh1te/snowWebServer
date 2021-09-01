{
    files = {
        "../EventLoop.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/EventLoop.cc.o:  ../EventLoop.cc ../EventLoop.h ../noncopyable.h ../Timestamp.h  ../CurrentThread.h ../TimerId.h ../Callbacks.h ../Logger.h ../Poller.h  ../Channel.h ../TimerQueue.h\
"
}