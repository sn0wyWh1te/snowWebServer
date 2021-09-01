{
    files = {
        "../TcpConnection.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/TcpConnection.cc.o:  ../TcpConnection.cc ../TcpConnection.h ../noncopyable.h ../InetAddress.h  ../Callbacks.h ../Buffer.h ../Timestamp.h ../Logger.h ../Socket.h  ../Channel.h ../EventLoop.h ../CurrentThread.h ../TimerId.h\
"
}