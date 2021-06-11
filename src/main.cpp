#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>
#include "test_timer.cpp" 

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

      m->type = msgType::TIME_REQUEST;
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


void func() {

    Unit1 server;
    server.waitForData();
}
int main() {
  boost::asio::io_context io;

  std::shared_ptr<msg> m = std::make_shared<msg>();
  auto duration = std::chrono::milliseconds(1000);

  AsioTimer timer(io, "asio_timer", duration,  m);
  AsioUnixSocket socket(io, "/tmp/asio_unix_socket", m);

  std::thread t1(func);

  while (io.run_one()) {

    if (m->event == eventName::TIMER)
      std::cout << "timer is: " << m->name<< std::endl;
    else if (m->event == eventName::UNIX_SOCKET)
      std::cout << "event is socket: " << m->name << std::endl;
    else
      std::cout << "unknown event" << std::endl;
  }

  return 0;

}