#ifndef ASYNC_PLATFORM_H
#define ASYNC_PLATFORM_H

#include <chrono>
#include <boost/asio.hpp>

#include "async_types.h"
#include "asio_timer.h"

namespace async_platform {

class AsyncPlatform {
public:
  AsyncPlatform(std::shared_ptr<msg> m);
  int waitForEvents();

private:
  boost::asio::io_context mIoContext;
  std::unique_ptr<AsioTimer> mTimer;

};


}
#endif