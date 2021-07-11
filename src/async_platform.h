#ifndef ASYNC_PLATFORM_H
#define ASYNC_PLATFORM_H

#include <chrono>
#include <vector>

#include <boost/asio.hpp>

#include "async_types.h"
#include "asio_timer.h"
#include "asio_unix_socket.h"

namespace async_platform {

class AsyncPlatform {
public:
  AsyncPlatform(std::string unitName, std::shared_ptr<msg> m, std::vector<timerData>& timerDataVect);
  int waitForEvents();
  void sendMsg(std::unique_ptr<socketData> d, std::string unitName) {
    mSocket->sendMsg(std::move(d), unitName);
  }

  std::shared_ptr<socketData> getSocketData() {
    return mSocket->getSocketData();
  }

private:
  std::unique_ptr<AsioUnixSocket> mSocket;
  boost::asio::io_context mIoContext;
  std::string mUnitName;
  std::vector<std::unique_ptr<AsioTimer>> mAsioTimersVect;

};


}
#endif