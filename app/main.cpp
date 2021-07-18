#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>

#include "async_types.h"
#include "async_platform.h"

void func1() {
  std::shared_ptr<msg> m = std::make_shared<msg>();
  std::vector<timerData> timerDataVect;
  async_platform::AsyncPlatform<socketData> plat("unit2", timerDataVect);

   while (true) {
     plat.waitForEvents();
     msg m = plat.getEventData();
     if (m.type == eventType::TIMER)
       std::cout << "thread: timer is: " << m.eventName << std::endl;
     else if (m.type == eventType::UNIX_SOCKET) {
      std::cout << "thread: event is socket: " << m.eventName << std::endl;
      std::cout << "thread: data: " << plat.getSocketData().data << std::endl;
     } else if (m.type == eventType::NONE) {
      
      std::cout << "none" << std::endl;
      continue;
     }
    else
      std::cout << "thread: what the heck is this??" << std::endl;
    
  }
}

int main() {

  std::shared_ptr<msg> m = std::make_shared<msg>();
  std::vector<timerData> timerDataVect;
  // auto duration = std::chrono::milliseconds(1000);
  auto duration1 = std::chrono::milliseconds(2000);
  // timerData dd;
  // dd.duration = duration;
  // dd.timerName = "timer1uu";
  timerData ddd;
  ddd.duration = duration1;
  ddd.timerName = "daff";
  // timerDataVect.push_back(dd);
  timerDataVect.push_back(ddd);
  
  async_platform::AsyncPlatform<socketData> plat("unit1", timerDataVect);
  std::thread t1(func1);

  while (true) {
    plat.waitForEvents();
    msg m = plat.getEventData();
    if (m.type == eventType::TIMER) {
      std::cout << "timer is: " << m.eventName<< std::endl;
      if (m.eventName == "daff") {
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
  
  
  return 0;

}