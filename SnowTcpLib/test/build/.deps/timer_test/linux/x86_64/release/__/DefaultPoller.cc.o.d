{
    files = {
        "../DefaultPoller.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/DefaultPoller.cc.o:  ../DefaultPoller.cc ../Poller.h ../noncopyable.h ../Timestamp.h  ../EPollPoller.h\
"
}