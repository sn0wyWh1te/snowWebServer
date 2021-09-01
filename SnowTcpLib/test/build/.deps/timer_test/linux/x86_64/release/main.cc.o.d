{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/main.cc.o: main.cc  ../EventLoop.h ../noncopyable.h ../Timestamp.h ../CurrentThread.h  ../TimerId.h ../Callbacks.h\
",
    files = {
        "main.cc"
    }
}