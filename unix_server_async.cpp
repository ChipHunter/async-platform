#include <iostream>
#include <string>
#include <string.h>
#include <array>
#include <functional>
#include <memory>
#include <thread>
#include <asio.hpp>

using asio::local::datagram_protocol;
enum class msgType { TIME, NUMBER };

struct msg {
  msgType type;
  int integer;
  std::string str;
};

std::string make_daytime_string() {

  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);

  return ctime(&now);

}


class UnixClient {
  public:
    UnixClient(std::string socketAddr) : mSocketAddr(socketAddr), mEndpoint("/tmp/unix_socket") {

      asio::io_context io_context;

      ::unlink(mSocketAddr.c_str());
      mpSocket = std::make_unique<datagram_protocol::socket>(io_context, datagram_protocol::endpoint(mSocketAddr));

    }

    void sendMsg(std::unique_ptr<msg> m) {
      char buf[sizeof(m)];
      msg* mm = m.release();
      memcpy(buf, &mm, sizeof(m));
      try {
        mpSocket->send_to(asio::buffer(buf), mEndpoint);
      } catch (std::exception& e) {
        delete mm;
        std::cerr << e.what() << std::endl;
      }
    }

    // size_t recvMsg(std::array<char, 128>& recvBuf) {

    //   datagram_protocol::endpoint sender_endpoint;
    //   return mpSocket->receive_from(
    //       asio::buffer(recvBuf), sender_endpoint);


  private:
    std::string mSocketAddr;
    datagram_protocol::endpoint mEndpoint;
    std::unique_ptr<datagram_protocol::socket> mpSocket;

};

void clientFunc() {

  int cnt = 0;
  while(true) {
    std::chrono::seconds sec(1);
    std::this_thread::sleep_for(sec);
    try {

      UnixClient client("/tmp/unix_client");
      std::unique_ptr<msg> m = std::make_unique<msg>();

      if (cnt%2==0) {
        m->type = msgType::NUMBER;
        m->integer = cnt;
      } else {
        m->type = msgType::TIME;
        m->str = make_daytime_string();
      }
      client.sendMsg(std::move(m));

    } catch (std::exception& e) {

      std::cerr << e.what() << std::endl;

    }

    cnt++;
  }

}

void func(std::unique_ptr<msg> m) {

  if (m->type == msgType::TIME)
    std::cout << "the time is: " << m->str << std::endl;
  if (m->type == msgType::NUMBER)
    std::cout << "the number is: " << m->integer << std::endl;

}

class UnixServer
{
public:
  UnixServer(std::function<void(std::unique_ptr<msg>)> func) : mFunc(func)
  {
    ::unlink("/tmp/unix_socket");
    mpSocket = std::make_unique<datagram_protocol::socket>(mIOContext, datagram_protocol::endpoint("/tmp/unix_socket"));
    start_receive();
  }

  void waitForData()  {
    mIOContext.run();
  }

private:
  void start_receive()
  {
    mpSocket->async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&UnixServer::handle_receive, this,
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
      mFunc(std::move(mm));


      // mpSocket->async_send_to(asio::buffer(*message), remote_endpoint_,
      //     std::bind(&UnixServer::handle_send, this, message,
      //       std::placeholders::_1,
      //       std::placeholders::_2));

      start_receive();
    }
  }

  void handle_send(std::shared_ptr<std::string> /*message*/,
      const asio::error_code& /*error*/,
      std::size_t /*bytes_transferred*/)
  {
  }


  private:
    std::unique_ptr<datagram_protocol::socket> mpSocket;
    datagram_protocol::endpoint remote_endpoint_;
    char recv_buffer_[sizeof(msg)];
    std::function<void(std::unique_ptr<msg>)> mFunc;
    asio::io_context mIOContext;
};

int main()
{
  try
  {

    UnixServer server(func);
    std::thread t1(clientFunc);

    server.waitForData();

    t1.join();

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
