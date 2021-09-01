#include "WebService.h"
#include "db.h"

WebService::WebService(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, double timeout)
    : server_(loop, listenAddr, nameArg, timeout)
{
    server_.setHttpCallBack(std::bind(&WebService::doRequest, this, std::placeholders::_1));
}

//格式user=123&password=123
static std::pair<std::string, std::string> getInfo(std::string &context)
{
    std::string name, passwd;
    int i;
    for (i = 5; context[i] != '&' && context[i] != '\0'; ++i)
    {
        name.push_back(context[i]);
    }

    for (i = i + 10; context[i] != '\0'; ++i)
    {
        passwd.push_back(context[i]);
    }
    return std::make_pair(name, passwd);
}

std::string WebService::doRequest(HttpConn *conn)
{
    //资源文件根目录
    char serverPath[200]{};
    getcwd(serverPath, 200);
    strcat(serverPath, "/root");

    std::string realFile = serverPath;
    std::string url = conn->getUrl();
    const char *p = strrchr(url.c_str(), '/');
    //该flag用于检测http请求到底是什么
    char flag = *(p + 1);

    HttpConn::METHOD method = conn->getMethod();

    //处理post
    if (method == HttpConn::METHOD::POST && (flag == '2' || flag == '3'))
    {
        //将用户名和密码提取出来
        std::string context = conn->getContext();
        std::pair<std::string, std::string> ID = getInfo(context);
        std::string &name = ID.first;
        std::string &passwd = ID.second;

        MySQL mysqldb;
        mysqldb.connect();

        char *sql_insert = (char *)malloc(sizeof(char) * 200);
        if (flag == '3')
        {
            //如果是注册，先检测数据库中是否有重名的
            //没有重名的，进行增加数据
            std::string sqlInsert("INSERT INTO user(username, passwd) VALUES(");
            sqlInsert += "'";
            sqlInsert += name;
            sqlInsert += "', '";
            sqlInsert += passwd;
            sqlInsert += "')";

            bool ret = mysqldb.update(sql_insert);
            if (ret)
                url = "/log.html";
            else
                url = "/registerError.html";
        }
        //如果是登录，直接判断
        //若浏览器端输入的用户名和密码在表中可以查找到，返回1，否则返回0
        else if (flag == '2')
        {
            char buffer[200]{};
            sprintf(buffer, "select * from user where username = '%s'", name.c_str());
            MYSQL_RES *res = mysqldb.query(buffer);
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                if (std::string(passwd) == std::string(row[1]))
                    url = "/welcome.html";
                else
                    url = "/logError.html";
            }
            else
                url = "/logError.html";
        }
    }

    if (flag == '0')
        realFile += "/register.html";

    else if (flag == '1')
        realFile += "/log.html";

    else if (flag == '5')
        realFile += "/picture.html";

    else if (flag == '6')
        realFile += "/video.html";

    else if (flag == '7')
        realFile += "/fans.html";
    else
        realFile += url;

    return realFile;
}

int main()
{
    EventLoop loop;
    InetAddress addr(9006, "0.0.0.0");
    WebService server(&loop, addr, "WebServer-01", 5);
    server.start();
    loop.loop();

    return 0;
}