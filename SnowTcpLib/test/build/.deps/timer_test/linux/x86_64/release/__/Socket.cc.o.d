{
    files = {
        "../Socket.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/Socket.cc.o: ../Socket.cc  ../Socket.h ../noncopyable.h ../Logger.h ../InetAddress.h\
"
}