# async-platform
async unix platform for multithreading based on ASIO. It is basically an abstarction layer on top of ASIO for providing OS services. Right now the only services provided are timers and unix sockets. MQTT services are planned for the near future. TCP, UDP and websockets might make it in. 

The idea behind async platform is to divide the appliacation into different threads and communicate between them using unix sockets. Each thread will sleep witing for data on the socket and when this happens, the application will get notified and will get the data on the socket. The application is responisible for defining the message structure that should be exchanged between the threads. 

Another service that the platform offers are timers. The application can define several timers when creating the async platform object. Each timer has a name and duration. When the timer expires, the platform will notify the application and deliver the name of the timer. 

The following is an example of how to use it: 

```C++

std::vector<timerData> timerDataVect;
auto duration1 = std::chrono::milliseconds(2000);
timerData td;
td.duration = duration1;
td.timerName = "timer1";
timerDataVect.push_back(td);
  
async_platform::AsyncPlatform<socketData> plat("unit1", timerDataVect);

while (true) {
    plat.waitForEvents();
    msg m = plat.getEventData();
    if (m.type == eventType::TIMER) {
      std::cout << "timer is: " << m.eventName<< std::endl;
      if (m.eventName == "timer1") {
        auto mm = std::make_unique<socketData>();
        mm->data = 333;
        plat.sendMsg(std::move(mm), "unit2");
      }
    }
    else if (m.type == eventType::UNIX_SOCKET)
      std::cout << "event is socket: " << m.eventName << std::endl;
    else if (m.type == eventType::NONE)
      continue;
    else
      std::cout << "what the heck is this??" << std::endl;
  }
  ```
  
  In the above example, a timer called "timer1" was defined. Every time the timer expires, a new message will be sent on the socket.
