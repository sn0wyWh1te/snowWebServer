# this is the build file for project 
# it is autogenerated by the xmake build system.
# do not edit by hand.

# project
cmake_minimum_required(VERSION 3.10.2)
project(webserver LANGUAGES C CXX ASM)

# target
add_executable(webserver "")
set_target_properties(webserver PROPERTIES OUTPUT_NAME "webserver")
set_target_properties(webserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY ".")
add_dependencies(webserver SnowTcpLib)
target_include_directories(webserver PRIVATE
    inc
    SnowTcpLib
    webServer
)
target_compile_options(webserver PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-m64>
    $<$<COMPILE_LANGUAGE:CXX>:-m64>
)
if(MSVC)
    target_compile_options(webserver PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(webserver PRIVATE -O0)
endif()
target_link_libraries(webserver PRIVATE
    mysqlclient
    SnowTcpLib
)
if(MSVC)
    target_link_libraries(webserver PRIVATE
        -libpath:/usr/lib/x86_64-linux-gnu
        -libpath:.
    )
else()
    target_link_libraries(webserver PRIVATE
        -L/usr/lib/x86_64-linux-gnu
        -L.
    )
endif()
target_link_libraries(webserver PRIVATE
    -m64
)
target_sources(webserver PRIVATE
    src/db.cc
    src/WebService.cc
    webServer/HttpConn.cc
    webServer/webServer.cc
    webServer/TimeWheel.cc
)

# target
add_library(SnowTcpLib SHARED "")
set_target_properties(SnowTcpLib PROPERTIES OUTPUT_NAME "SnowTcpLib")
set_target_properties(SnowTcpLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ".")
target_include_directories(SnowTcpLib PRIVATE
    inc
    SnowTcpLib
    webServer
)
target_compile_options(SnowTcpLib PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-m64>
    $<$<COMPILE_LANGUAGE:CXX>:-m64>
)
if(MSVC)
    target_compile_options(SnowTcpLib PRIVATE $<$<CONFIG:Debug>:-Od>)
else()
    target_compile_options(SnowTcpLib PRIVATE -O0)
endif()
target_link_libraries(SnowTcpLib PRIVATE
    pthread
)
target_link_libraries(SnowTcpLib PRIVATE
    -m64
)
target_sources(SnowTcpLib PRIVATE
    SnowTcpLib/Buffer.cc
    SnowTcpLib/Channel.cc
    SnowTcpLib/EventLoopThread.cc
    SnowTcpLib/Socket.cc
    SnowTcpLib/DefaultPoller.cc
    SnowTcpLib/TimerQueue.cc
    SnowTcpLib/Poller.cc
    SnowTcpLib/CurrentThread.cc
    SnowTcpLib/TcpServer.cc
    SnowTcpLib/Timestamp.cc
    SnowTcpLib/Logger.cc
    SnowTcpLib/Acceptor.cc
    SnowTcpLib/EventLoopThreadPool.cc
    SnowTcpLib/TcpConnection.cc
    SnowTcpLib/EPollPoller.cc
    SnowTcpLib/EventLoop.cc
    SnowTcpLib/InetAddress.cc
    SnowTcpLib/Timer.cc
    SnowTcpLib/Thread.cc
)

