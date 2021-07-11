#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>

#include "async_types.h"
#include "async_platform.h"

using boost::asio::local::datagram_protocol;
class Unit1
{
public:
  Unit1()
  {
    ::unlink("/tmp/unix_socket1");
    mpSocket = std::make_unique<datagram_protocol::socket>(mIOContext, datagram_protocol::endpoint("/tmp/unix_socket1"));
    mpTimer = std::make_unique<boost::asio::steady_timer>(mIOContext, std::chrono::seconds(5));
    start_receive();
    start_timer();
  }

  void waitForData()  {
    mIOContext.run();
  }

  void sendMsg(std::unique_ptr<msg> m, datagram_protocol::endpoint address) {
    char buf[sizeof(m)];
    msg* mm = m.release();
    memcpy(buf, &mm, sizeof(m));
    try {
      mpSocket->send_to(boost::asio::buffer(buf), address);
    } catch (std::exception& e) {
      delete mm;
      std::cerr << e.what() << std::endl;
    }
  }


private:
  void handle_timer(const boost::system::error_code& /*e*/)
  {
    std::cout << "sending request!" << std::endl;
    try {

      std::unique_ptr<msg> m = std::make_unique<msg>();

      // m->type = msgType::TIME_REQUEST;
      sendMsg(std::move(m), datagram_protocol::endpoint("/tmp/asio_unix_socket"));

    } catch (std::exception& e) {

      std::cerr << e.what() << std::endl;

    }
    start_timer();
  }

  void start_receive()
  {
    mpSocket->async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&Unit1::handle_receive, this,
          std::placeholders::_1,
          std::placeholders::_2));
  }

  void start_timer()
  {
    mpTimer->expires_at(mpTimer->expiry() + std::chrono::seconds(5));
    mpTimer->async_wait(std::bind(&Unit1::handle_timer, this, std::placeholders::_1));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error)
    {
      msg* m;
      memcpy(&m, recv_buffer_, sizeof(m));
      std::unique_ptr<msg> mm(m);
      /* if(mm->type == msgType::TIME_REPLY) */
      /*   std::cout << "time is: " << mm->str << std::endl; */

      start_receive();
    }
  }

  private:
    std::unique_ptr<datagram_protocol::socket> mpSocket;
    std::unique_ptr<boost::asio::steady_timer> mpTimer;
    datagram_protocol::endpoint mServerAddress = datagram_protocol::endpoint("/tmp/asio_unix_socket");
    datagram_protocol::endpoint remote_endpoint_;
    char recv_buffer_[sizeof(msg)];
    boost::asio::io_context mIOContext;
};


void func1() {
  std::shared_ptr<msg> m = std::make_shared<msg>();
  std::vector<timerData> timerDataVect;
  async_platform::AsyncPlatform plat("unit2", m, timerDataVect);

   while (true) {
     std::cout << "thread: waiting! " << m->eventName << std::endl;
     plat.waitForEvents();
     if (m->type == eventType::TIMER)
       std::cout << "thread: timer is: " << m->eventName << std::endl;
     else if (m->type == eventType::UNIX_SOCKET) {
      std::cout << "thread: event is socket: " << m->eventName << std::endl;
      std::cout << "thread: data: " << plat.getSocketData()->data << std::endl;
     } else if (m->type == eventType::NONE) {
      
      std::cout << "none" << std::endl;
      continue;
     }
    else
      std::cout << "thread: what the heck is this??" << std::endl;
    
    m->type = eventType::NONE;
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
  
  async_platform::AsyncPlatform plat("unit1", m, timerDataVect);
  std::thread t1(func1);

  while (true) {
    plat.waitForEvents();
    if (m->type == eventType::TIMER) {
      std::cout << "timer is: " << m->eventName<< std::endl;
      if (m->eventName == "daff") {
        auto mm = std::make_unique<socketData>();
        mm->data = 333;
        plat.sendMsg(std::move(mm), "unit2");
      }
    }
    else if (m->type == eventType::UNIX_SOCKET)
      std::cout << "event is socket: " << m->eventName << std::endl;
    else if (m->type == eventType::NONE)
      continue;
    else
      std::cout << "what the heck is this??" << std::endl;
    m->type = eventType::NONE;
  }
  
  
  return 0;

}