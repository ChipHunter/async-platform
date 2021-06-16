#include "asio_unix_socket.h"

namespace async_platform {

AsioUnixSocket::AsioUnixSocket(boost::asio::io_context& io, std::string name, std::shared_ptr<msg> m) : 
                                                      mSocketName(name) , mSocketMsg(m) {
  ::unlink(mSocketName.c_str());
  mpSocket = std::make_unique<boost::asio::local::datagram_protocol::socket>(io, 
                          boost::asio::local::datagram_protocol::endpoint(mSocketName));
  start_receive();
}

void AsioUnixSocket::start_receive()
{
  mpSocket->async_receive_from(
      boost::asio::buffer(recv_buffer_), mServerAddress,
      std::bind(&AsioUnixSocket::handle_receive, this,
        std::placeholders::_1,
        std::placeholders::_2,
        mSocketMsg, mSocketName));
}

void AsioUnixSocket::handle_receive(const boost::system::error_code& error,
    std::size_t, std::shared_ptr<msg> mm, std::string name)
{
  if (!error)
  {
    std::cout << "handle receive!" << std::endl;
    msg* m;
    memcpy(&m, recv_buffer_, sizeof(m));
    std::unique_ptr<msg> mmm(m);
    if (mmm->type == msgType::TIME_REQUEST) {
      std::cout << "inside" << std::endl;
      mm->event = eventName::UNIX_SOCKET;
      mm->name = name;
    }

    start_receive();
  }
}

} //namespace async_platform