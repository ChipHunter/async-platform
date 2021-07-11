#ifndef ASYNC_PLATFORM_ASIO_UNIX_SOCKET_H
#define ASYNC_PLATFORM_ASIO_UNIX_SOCKET_H
#include <string>
#include <memory>
#include <chrono>
#include <iostream>

#include <boost/asio.hpp>

#include "async_types.h"

namespace async_platform {

class AsioUnixSocket {
public:
  AsioUnixSocket(boost::asio::io_context& io, std::string name, std::shared_ptr<msg> m); 
  void sendMsg(std::unique_ptr<socketData> d, std::string unitName);
  const std::shared_ptr<socketData> getSocketData() {
    return mSocketData; 
  }

private:
  std::unique_ptr<boost::asio::local::datagram_protocol::socket> mpSocket;
  boost::asio::local::datagram_protocol::endpoint mServerAddress;
  char recv_buffer_[sizeof(msg)];
  std::string mSocketName;
  std::string mSocketPath;
  std::shared_ptr<msg> mSocketMsg;
  std::shared_ptr<socketData> mSocketData;

private:
  void start_receive();

  void handle_receive(const boost::system::error_code& error,
      std::size_t, std::shared_ptr<msg> mm, std::string name);
};

} //namespace async_plat

#endif