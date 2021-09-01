{
    files = {
        "../Timer.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/Timer.cc.o: ../Timer.cc  ../Timer.h ../noncopyable.h ../Timestamp.h ../Callbacks.h\
"
}