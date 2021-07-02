#ifndef ASYNC_PLATFORM_H
#define ASYNC_PLATFORM_H

#include <chrono>
#include <boost/asio.hpp>

#include "async_types.h"
#include "asio_timer.h"
#include "asio_unix_socket.h"

namespace async_platform {

class AsyncPlatform {
public:
  AsyncPlatform(std::string unitName, std::shared_ptr<msg> m);
  int waitForEvents();

private:
  boost::asio::io_context mIoContext;
  std::string mUnitName;
  std::unique_ptr<AsioTimer> mTimer;
  std::unique_ptr<AsioUnixSocket> mSocket;

};


}
#endif