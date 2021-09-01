{
    files = {
        "../TimerQueue.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/TimerQueue.cc.o:  ../TimerQueue.cc ../TimerQueue.h ../Timestamp.h ../Callbacks.h  ../Channel.h ../noncopyable.h ../Logger.h ../Timer.h ../EventLoop.h  ../CurrentThread.h ../TimerId.h\
"
}