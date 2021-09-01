#include "HttpConn.h"
#include "db.h"

#include <mysql/mysql.h>
#include <fstream>
#include <string>

//定义http响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";

//初始化新接受的连接
//check_state默认为分析请求行状态
void HttpConn::init(httpCallBack &cb)
{
    checkState_ = CHECK_STATE_REQUESTLINE;
    linger_ = false;
    method_ = GET;
    contentLength_ = 0;
    startLine_ = 0;
    checkedIndex_ = 0;
    readIndex_ = 0;
    alive_ = false;
    cb_ = cb;
}

//从状态机，用于分析出一行内容
//返回值为行的读取状态，有LINE_OK,LINE_BAD,LINE_OPEN
HttpConn::LINE_STATUS HttpConn::parseLine()
{
    char temp;
    for (; checkedIndex_ < readIndex_; ++checkedIndex_)
    {
        temp = (*readBuff_)[checkedIndex_];
        if (temp == '\r')
        {
            if ((checkedIndex_ + 1) == readIndex_)
                return LINE_OPEN;
            else if ((*readBuff_)[checkedIndex_ + 1] == '\n')
            {
                (*readBuff_)[checkedIndex_++] = '\0';
                (*readBuff_)[checkedIndex_++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        else if (temp == '\n')
        {
            if (checkedIndex_ > 1 && (*readBuff_)[checkedIndex_ - 1] == '\r')
            {
                (*readBuff_)[checkedIndex_ - 1] = '\0';
                (*readBuff_)[checkedIndex_++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

//解析http请求行，获得请求方法，目标url及http版本号
HttpConn::HTTP_CODE HttpConn::parseRequestLine(char *text)
{
    char *version = nullptr;
    char *url = nullptr;

    url = strpbrk(text, " \t");
    if (!url)
    {
        return BAD_REQUEST;
    }
    *url++ = '\0';
    char *method = text;
    if (strcasecmp(method, "GET") == 0)
        method_ = GET;
    else if (strcasecmp(method, "POST") == 0)
    {
        method_ = POST;
    }
    else
        return BAD_REQUEST;
    url += strspn(url, " \t");
    version = strpbrk(url, " \t");
    if (!version)
        return BAD_REQUEST;
    *version++ = '\0';
    version += strspn(version, " \t");
    if (strcasecmp(version, "HTTP/1.1") != 0)
        return BAD_REQUEST;
    version_ = version;

    if (strncasecmp(url, "http://", 7) == 0)
    {
        url += 7;
        url = strchr(url, '/');
    }

    if (strncasecmp(url, "https://", 8) == 0)
    {
        url += 8;
        url = strchr(url, '/');
    }

    if (!url || url[0] != '/')
        return BAD_REQUEST;
    //当url为/时，显示判断界面
    if (strlen(url) == 1)
        strcat(url, "judge.html");

    url_ = url;
    checkState_ = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

//解析http请求的一个头部信息
HttpConn::HTTP_CODE HttpConn::parseHeaders(char *text)
{
    if (text[0] == '\0')
    {
        if (contentLength_ != 0)
        {
            checkState_ = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive") == 0)
        {
            linger_ = true;
        }
    }
    else if (strncasecmp(text, "Content-length:", 15) == 0)
    {
        text += 15;
        text += strspn(text, " \t");
        contentLength_ = atol(text);
    }
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        text += 5;
        text += strspn(text, " \t");
        host_ = text;
    }
    else
    {
        //LOG_INFO("oop!unknow header: %s", text);
    }
    return NO_REQUEST;
}

//判断http请求是否被完整读入
HttpConn::HTTP_CODE HttpConn::parseContent(char *text)
{
    if (readIndex_ >= (contentLength_ + checkedIndex_))
    {
        text[contentLength_] = '\0';
        //POST请求中的body内容
        bodyAsString_ = text;
        return GET_REQUEST;
    }
    return NO_REQUEST;
}

HttpConn::HTTP_CODE HttpConn::processRead()
{
    LINE_STATUS lineStatus = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char *text = nullptr;

    while ((checkState_ == CHECK_STATE_CONTENT && lineStatus == LINE_OK) || ((lineStatus = parseLine()) == LINE_OK))
    {
        text = getLine();
        startLine_ = checkedIndex_;
        switch (checkState_)
        {
        case CHECK_STATE_REQUESTLINE:
        {
            ret = parseRequestLine(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            break;
        }
        case CHECK_STATE_HEADER:
        {
            ret = parseHeaders(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            else if (ret == GET_REQUEST)
            {
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT:
        {
            ret = parseContent(text);
            if (ret == GET_REQUEST)
                return do_request();
            lineStatus = LINE_OPEN;
            break;
        }
        default:
            return INTERNAL_ERROR;
        }
    }
    return NO_REQUEST;
}

HttpConn::HTTP_CODE HttpConn::do_request()
{
    if (!cb_)
    {
        //LOG<<
    }
    realFile_ = cb_(this);
    if (stat(realFile_.c_str(), &fileStat_) < 0)
        return NO_RESOURCE;

    if (!(fileStat_.st_mode & S_IROTH))
        return FORBIDDEN_REQUEST;

    if (S_ISDIR(fileStat_.st_mode))
        return BAD_REQUEST;

    return FILE_REQUEST;
}

bool HttpConn::add_response(const char *format, ...)
{
    char buffer[WRITE_BUFFER_SIZE]{};

    va_list arg_list;
    va_start(arg_list, format);
    int len = vsnprintf(buffer, WRITE_BUFFER_SIZE - 1, format, arg_list);
    if (len >= WRITE_BUFFER_SIZE)
    {
        va_end(arg_list);
        return false;
    }
    va_end(arg_list);

    writeBuff_ += buffer;
    return true;
}

bool HttpConn::add_status_line(int status, const char *title)
{
    return add_response("%s %d %s\r\n", "HTTP/1.1", status, title);
}

bool HttpConn::add_headers(int content_len)
{
    return add_content_length(content_len) && add_linger() &&
           add_blank_line();
}

bool HttpConn::add_content_length(int content_len)
{
    return add_response("Content-Length:%d\r\n", content_len);
}

bool HttpConn::add_content_type()
{
    return add_response("Content-Type:%s\r\n", "text/html");
}
bool HttpConn::add_linger()
{
    return add_response("Connection:%s\r\n", (linger_ == true) ? "keep-alive" : "close");
}
bool HttpConn::add_blank_line()
{
    return add_response("%s", "\r\n");
}
bool HttpConn::add_content(const char *content)
{
    return add_response("%s", content);
}
bool HttpConn::process_write(HTTP_CODE ret)
{
    switch (ret)
    {
    case INTERNAL_ERROR:
    {
        add_status_line(500, error_500_title);
        add_headers(strlen(error_500_form));
        if (!add_content(error_500_form))
            return false;
        break;
    }
    case BAD_REQUEST:
    {
        add_status_line(404, error_404_title);
        add_headers(strlen(error_404_form));
        if (!add_content(error_404_form))
            return false;
        break;
    }
    case FORBIDDEN_REQUEST:
    {
        add_status_line(403, error_403_title);
        add_headers(strlen(error_403_form));
        if (!add_content(error_403_form))
            return false;
        break;
    }
    case FILE_REQUEST:
    {
        add_status_line(200, ok_200_title);
        if (fileStat_.st_size != 0)
        {
            add_headers(fileStat_.st_size);
            return true;
        }
        else
        {
            const char *ok_string = "<html><body></body></html>";
            add_headers(strlen(ok_string));
            if (!add_content(ok_string))
                return false;
        }
    }
    default:
        return false;
    }
    return true;
}

HttpConn::HTTP_CODE HttpConn::process(std::string *msg)
{
    readBuff_ = msg;
    readIndex_ = msg->length();
    HTTP_CODE read_ret = processRead();
    if (read_ret == NO_REQUEST)
    {
        return NO_REQUEST;
    }
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        return BAD_REQUEST;
    }
    return GET_REQUEST;
}