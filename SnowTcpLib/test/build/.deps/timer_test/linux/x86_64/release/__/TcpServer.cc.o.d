{
    files = {
        "../TcpServer.cc"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.."
        }
    },
    depfiles_gcc = "build/.objs/timer_test/linux/x86_64/release/__/TcpServer.cc.o:  ../TcpServer.cc ../TcpServer.h ../EventLoop.h ../noncopyable.h  ../Timestamp.h ../CurrentThread.h ../TimerId.h ../Callbacks.h  ../Acceptor.h ../Socket.h ../Channel.h ../InetAddress.h  ../EventLoopThreadPool.h ../TcpConnection.h ../Buffer.h ../Logger.h\
"
}