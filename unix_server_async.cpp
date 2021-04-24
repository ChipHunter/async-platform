#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <memory>
#include <thread>
#include <asio.hpp>

using asio::local::datagram_protocol;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class udp_server
{
public:
  udp_server(asio::io_context& io_context)
  {
    ::unlink("/tmp/unix_socket");
    socket_ = new  datagram_protocol::socket(io_context, datagram_protocol::endpoint("/tmp/unix_socket"));
    start_receive();
  }

  ~udp_server() {

      delete socket_;

  }

private:
  void start_receive()
  {
    socket_->async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&udp_server::handle_receive, this,
          std::placeholders::_1,
          std::placeholders::_2));
  }

  void handle_receive(const asio::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error)
    {
      std::shared_ptr<std::string> message(
          new std::string(make_daytime_string()));

      socket_->async_send_to(asio::buffer(*message), remote_endpoint_,
          std::bind(&udp_server::handle_send, this, message,
            std::placeholders::_1,
            std::placeholders::_2));

//      start_receive();
    }
  }

  void handle_send(std::shared_ptr<std::string> /*message*/,
      const asio::error_code& /*error*/,
      std::size_t /*bytes_transferred*/)
  {
  }

  datagram_protocol::socket* socket_;
  datagram_protocol::endpoint remote_endpoint_;
  std::array<char, 1> recv_buffer_;
};

int main()
{
  try
  {
    asio::io_context io_context;
    udp_server server(io_context);
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
