#pragma once

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <vector>
#include <functional>
#include <atomic>

#include "noncopyable.h"

class HttpConn : noncopyable
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    using httpCallBack = std::function<std::string(HttpConn *conn)>;

    HttpConn() {}
    ~HttpConn() {}

    HTTP_CODE process(std::string *msg);
    void init(httpCallBack &cb);
    std::string getFilePath() { return realFile_; };
    std::string getResponseHead() { return writeBuff_; };
    std::string getUrl() { return std::string(url_); }
    METHOD getMethod() { return method_; }
    std::string getContext() { return bodyAsString_; }
    int checkAlive() { return alive_; }
    void setAlive(bool alive) { alive_ = alive; }

private:
    HTTP_CODE processRead();
    bool process_write(HTTP_CODE ret);
    HTTP_CODE parseRequestLine(char *text);
    HTTP_CODE parseHeaders(char *text);
    HTTP_CODE parseContent(char *text);
    HTTP_CODE do_request();
    char *getLine() { return &(*readBuff_)[startLine_]; };
    LINE_STATUS parseLine();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

private:
    int readIndex_;
    int checkedIndex_;
    int startLine_;
    CHECK_STATE checkState_;
    METHOD method_;
    int contentLength_;
    bool linger_;
    struct stat fileStat_;
    std::atomic_bool alive_;

    std::string *readBuff_;
    std::string realFile_;
    std::string writeBuff_;
    httpCallBack cb_;

    std::string url_;
    std::string version_;
    std::string host_;
    std::string bodyAsString_;
};