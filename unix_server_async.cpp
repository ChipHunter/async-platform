#include <iostream>
#include <string>
#include <string.h>
#include <array>
#include <functional>
#include <memory>
#include <thread>
#include <asio.hpp>

using asio::local::datagram_protocol;

struct msg {

   //enum class msgType { TYPE1, TYPE2 };
   int integer;
   std::string str;

};

std::string make_daytime_string()
{
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

    //void sendMsg(std::array<char, 128>& sendBuf) {
    void sendMsg(msg* m) {

      char buf[sizeof(msg)];
      memcpy(buf, m, sizeof(msg));

      mpSocket->send_to(asio::buffer(buf), mEndpoint);

    }

    // size_t recvMsg(std::array<char, 128>& recvBuf) {

    //   datagram_protocol::endpoint sender_endpoint;
    //   return mpSocket->receive_from(
    //       asio::buffer(recvBuf), sender_endpoint);

    // }

  private:
    std::string mSocketAddr;
    datagram_protocol::endpoint mEndpoint;
    std::unique_ptr<datagram_protocol::socket> mpSocket;

};

void clientFunc() {

  std::chrono::seconds sec(1);
  std::this_thread::sleep_for(sec);
  try {

    UnixClient client("/tmp/unix_client");
    //std::array<char, 128> sendBuf = {0};
    msg m;
    m.integer = 1;
    m.str = "Hi";
    client.sendMsg(&m);

    // std::array<char, 128> recv_buf;
    // size_t len = client.recvMsg(recv_buf);
    // std::cout.write(recv_buf.data(), len);

  } catch (std::exception& e) {

    std::cerr << e.what() << std::endl;

  }

}

class UnixServer
{
public:
  UnixServer()
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
      // std::shared_ptr<std::string> message(
      //     new std::string(make_daytime_string()));

      msg m;
      memcpy(&m, recv_buffer_, sizeof(msg));
      std::cout << "integer is: " << m.integer << std::endl 
                << "string is: " << m.str << std::endl;

      // mpSocket->async_send_to(asio::buffer(*message), remote_endpoint_,
      //     std::bind(&UnixServer::handle_send, this, message,
      //       std::placeholders::_1,
      //       std::placeholders::_2));

      // start_receive();
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
    //std::array<char, 1> recv_buffer_;
    char recv_buffer_[sizeof(msg)];
    asio::io_context mIOContext;
};

int main()
{
  try
  {

    UnixServer server;
    std::thread t1(clientFunc);

    server.waitForData();

    t1.join();

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
