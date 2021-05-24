#include <iostream>
#include <string>
#include <string.h>
#include <array>
#include <functional>
#include <memory>
#include <thread>
#include <asio.hpp>

using asio::local::datagram_protocol;
enum class msgType { TIME_REQUEST, TIME_REPLY };

struct msg {
  msgType type;
  std::string str;
};

std::string make_daytime_string() {

  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);

  return ctime(&now);

}


class Unit2
{
public:
  Unit2()
  {
    ::unlink("/tmp/unix_socket2");
    mpSocket = std::make_unique<datagram_protocol::socket>(mIOContext, datagram_protocol::endpoint("/tmp/unix_socket2"));
    start_receive();
  }

  void waitForData()  {
    mIOContext.run();
  }

  void sendMsg(std::unique_ptr<msg> m, datagram_protocol::endpoint address) {
    char buf[sizeof(m)];
    msg* mm = m.release();
    memcpy(buf, &mm, sizeof(m));
    try {
      mpSocket->send_to(asio::buffer(buf), address);
    } catch (std::exception& e) {
      delete mm;
      std::cerr << e.what() << std::endl;
    }
  }


private:
  void start_receive()
  {
    mpSocket->async_receive_from(
        asio::buffer(recv_buffer_), mServerAddress,
        std::bind(&Unit2::handle_receive, this,
          std::placeholders::_1,
          std::placeholders::_2));
  }

  void handle_receive(const asio::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error)
    {
      msg* m;
      memcpy(&m, recv_buffer_, sizeof(m));
      std::unique_ptr<msg> mm(m);
      if (mm->type == msgType::TIME_REQUEST) {
        mm->type = msgType::TIME_REPLY;
        mm->str = make_daytime_string();
        sendMsg(std::move(mm), mServerAddress);
      }

      start_receive();
    }
  }


  private:
    std::unique_ptr<datagram_protocol::socket> mpSocket;
    std::unique_ptr<asio::steady_timer> mpTimer;
    datagram_protocol::endpoint mServerAddress;
    char recv_buffer_[sizeof(msg)];
    asio::io_context mIOContext;
};

void clientFunc() {

  Unit2 server;
  server.waitForData();

}
class Unit1
{
public:
  Unit1()
  {
    ::unlink("/tmp/unix_socket1");
    mpSocket = std::make_unique<datagram_protocol::socket>(mIOContext, datagram_protocol::endpoint("/tmp/unix_socket1"));
    mpTimer = std::make_unique<asio::steady_timer>(mIOContext, asio::chrono::seconds(5));
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
      mpSocket->send_to(asio::buffer(buf), address);
    } catch (std::exception& e) {
      delete mm;
      std::cerr << e.what() << std::endl;
    }
  }


private:
  void handle_timer(const asio::error_code& /*e*/)
  {
    std::cout << "sending request!" << std::endl;
    try {

      std::unique_ptr<msg> m = std::make_unique<msg>();

      m->type = msgType::TIME_REQUEST;
      sendMsg(std::move(m), mServerAddress);

    } catch (std::exception& e) {

      std::cerr << e.what() << std::endl;

    }
    start_timer();
  }

  void start_receive()
  {
    mpSocket->async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&Unit1::handle_receive, this,
          std::placeholders::_1,
          std::placeholders::_2));
  }

  void start_timer()
  {
    mpTimer->expires_at(mpTimer->expiry() + asio::chrono::seconds(5));
    mpTimer->async_wait(std::bind(&Unit1::handle_timer, this, std::placeholders::_1));
  }

  void handle_receive(const asio::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error)
    {
      msg* m;
      memcpy(&m, recv_buffer_, sizeof(m));
      std::unique_ptr<msg> mm(m);
      if(mm->type == msgType::TIME_REPLY)
        std::cout << "time is: " << mm->str << std::endl;

      start_receive();
    }
  }

  private:
    std::unique_ptr<datagram_protocol::socket> mpSocket;
    std::unique_ptr<asio::steady_timer> mpTimer;
    datagram_protocol::endpoint mServerAddress = datagram_protocol::endpoint("/tmp/unix_socket2");
    datagram_protocol::endpoint remote_endpoint_;
    char recv_buffer_[sizeof(msg)];
    asio::io_context mIOContext;
};

int main()
{
  try
  {

    Unit1 server;
    std::thread t1(clientFunc);

    server.waitForData();

    t1.join();

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
