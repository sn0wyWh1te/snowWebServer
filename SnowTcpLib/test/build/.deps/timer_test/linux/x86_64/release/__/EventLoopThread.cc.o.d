{
    files = {
        "../EventLoopThread.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/EventLoopThread.cc.o:  ../EventLoopThread.cc ../EventLoopThread.h ../noncopyable.h ../Thread.h  ../EventLoop.h ../Timestamp.h ../CurrentThread.h ../TimerId.h  ../Callbacks.h\
"
}