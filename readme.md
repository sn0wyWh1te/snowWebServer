该代码为一个轻量级的webserver
    参考了muduo的reactor模型，由于muduo采用的boost库在在新标准中已经引入，所以本项目对muduo的boost库依赖进行了删减，已经不依赖boost库了

    muduo使用linux下POSIX标准的pthread库，本项目修改为使用现代c++线程库

使用方式
    第一步建立数据库：
    // 建立yourdb库
    create database yourdb;

    // 创建user表
    USE yourdb;
    CREATE TABLE user(
        username char(50) NULL,
        passwd char(50) NULL
    )ENGINE=InnoDB;


    然后修改main函数中的ip和端口号为自己需要
    其次需要在db.cc中修改四个全局静态变量，这四个变量为连接数据库的ip,account,password,Name of DataBase
    最后用makefile进行构建项目（也支持用xmake构建项目）

目前支持功能
    支持POST和GET方法
    支持keep-alive，并定时剔除不活跃链接
    发送大文件采用mmap提高性能  

压力测试
    GET方法的qps到达顶峰时，并发量为15000左右

鸣谢：muduo网络库作者陈硕，《高性能服务器开发》作者游双
    

