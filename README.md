# 手把手教你在Tars框架里使用protobuf (cpp版)

在一台Linux服务器上搭建开发环境，按说明安装好 tars, protobuf 等

新建一个项目目录，里面放
```
.
├── flight.proto             // pb协议定义文件
├── makefile                 // 符合tars规范的makefile
└── protobuf-gen.sh          // 生成pb对象代码
```

运行 protobuf-gen.sh 后，出现新的文件，我们只用 cpp 的

```
.
├── flight.pb.cc             // 新文件
├── flight.pb.go             // 新文件
├── flight.pb.h              // 新文件
├── flight.proto
├── makefile
└── protobuf-gen.sh
```

## 先编写 cpp 的服务端

新建文件 MyProtoBufApp.cpp 注意这是包含 main 的主要 cpp 文件，**命名最好与 makefile 里定义的 TARGET 一致**。
```
.
├── flight.pb.cc
├── flight.pb.go
├── flight.pb.h
├── flight.proto
├── makefile
├── MyProtoBufApp.cpp        // 新文件
└── protobuf-gen.sh
```

在  MyProtoBufApp.cpp 里添加 main 函数和 Application 实现类。一个最精简的实现如下：

http://code.yy.com/friday/yytars-protobuf/blob/f7cc4d577f0b5604a51597b603779eb6ca3a1262/MyProtoBufApp.cpp

现在打 make ，然后运行 ./MyProtoBufApp --config=config.conf，观察 /usr/local/app/tars/app_log/ProtoBufDemo/MyProtoBufApp/ProtoBufDemo.MyProtoBufApp.log 里的日志

```
2018-01-18 11:24:55|32641|DEBUG|Enter virtual void MyProtoBufApp::initialize()
2018-01-18 11:24:55|32650|DEBUG|Enter virtual void FlightService::initialize()
2018-01-18 11:24:55|32651|DEBUG|Enter virtual void FlightService::initialize()
2018-01-18 11:24:55|32652|DEBUG|Enter virtual void FlightService::initialize()
2018-01-18 11:24:55|32653|DEBUG|Enter virtual void FlightService::initialize()
2018-01-18 11:24:55|32654|DEBUG|Enter virtual void FlightService::initialize()
2018-01-18 11:25:31|32641|DEBUG|Enter virtual void MyProtoBufApp::destroyApp()
```

因为在配置文件 config.conf 里配了 5 个处理线程，所以 FlightService 初始化 5 次 （5 个对象）
目前，这个应用还无法处理任何消息。我们在 FlightService 类里添加虚函数 onDispatch 的 override, 收到消息时，打一条日志，显示函数名和消息体字节数。

```cpp
virtual int onDispatch(TarsCurrentPtr current, vector<char> &buffer)
{
    const vector<char> & req = current->getRequestBuffer();
    TLOGDEBUG(__PRETTY_FUNCTION__ << " request.FuncName=" << current->getFuncName()
                                  << " request.DataSize=" << req.size()
                                  << endl);
    return TARSSERVERSUCCESS;
}
```

## 编写 cpp 的客户端，向服务端发消息测试

新建一个子目录，共用上级目录的 flight.pb.h 文件。
```
.
├── client
│   ├── makefile              // 新文件
│   └── MyProtoBufClient.cpp  // 新文件
├── config.conf
├── flight.pb.cc
├── flight.pb.go
├── flight.pb.h
├── flight.proto
├── makefile
├── MyProtoBufApp
├── MyProtoBufApp.cpp
├── MyProtoBufApp.o
└── protobuf-gen.sh
```

一个最精简的，支持 protobuf 的客户端实现如下：

http://code.yy.com/friday/yytars-protobuf/blob/master/client/MyProtoBufClient.cpp

现在打 make ，然后运行 ./MyProtoBufClient

客户端日志：
```
[libprotobuf ERROR google/protobuf/message_lite.cc:121] Can't parse message of type "com.yy.tars.demo.flight.QueryFlightResponse" because it is missing required fields: errcode, errmsg, flightsInfo
MyProtoBufClient: MyProtoBufClient.cpp:28: void ProtoBufProxy::proto_invoke(const string&, const google::protobuf::Message&, google::protobuf::Message&): Assertion `ok' failed.
Aborted (core dumped)
```

服务端日志：
```
2018-01-18 16:03:40|1351|DEBUG|[TARS]accept [172.26.66.43:42440] [16] incomming
2018-01-18 16:03:40|1346|DEBUG|virtual int FlightService::onDispatch(tars::TarsCurrentPtr, std::vector<char>&) request.FuncName=queryFlight request.DataSize=30
2018-01-18 16:03:40|1351|DEBUG|[TARS]recv [172.26.66.43:42440] close connection
```

说明服务端已经收到对 `FlightService` 对象的 `queryFlight` 调用。protobuf 编码的请求长度为 30 字节。
由于服务端没有解包和回应，客户端报错。

## 在服务端解包和回应

现在我们在 MyProtoBufApp.cpp 里添加解包和回应的相关逻辑，完整的实现在

http://code.yy.com/friday/yytars-protobuf/blob/master/MyProtoBufApp.cpp

再次运行，

客户端日志：
```
2018-01-18 16:19:54|CommunicatorEpoll::run id:1670
queryFlight: errcode=0 errmsg=Success. flightsInfo=XT3697
```

服务端日志：
```
2018-01-18 16:19:54|1661|DEBUG|[TARS]accept [172.26.66.43:42481] [17] incomming
2018-01-18 16:19:54|1658|DEBUG|virtual int FlightService::onDispatch(tars::TarsCurrentPtr, std::vector<char>&) request.FuncName=queryFlight request.DataSize=30
2018-01-18 16:19:54|1658|DEBUG|virtual int FlightService::onDispatch(tars::TarsCurrentPtr, std::vector<char>&) request.Data.name=Leo request.Data.password=123 request.Data.departureCity=Gua
ngzhou request.Data.destinationCity=Beijing
2018-01-18 16:19:54|1661|DEBUG|[TARS]recv [172.26.66.43:42481] close connection
```

至此，一个完整且最精简的基于 protobuf 协议的Tars框架rpc调用就完成了。

