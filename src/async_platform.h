#ifndef ASYNC_PLATFORM_H
#define ASYNC_PLATFORM_H

#include <chrono>
#include <vector>

#include <boost/asio.hpp>

#include "async_types.h"
#include "asio_timer.h"
#include "asio_unix_socket.h"

namespace async_platform {

template <class T>
class AsyncPlatform {
public:
  AsyncPlatform(std::string unitName, 
                                std::vector<timerData>& timerDataVect)  : 
                                mUnitName(unitName) { 

    mEventData = std::make_shared<msg>();

    for (auto i : timerDataVect) {

      auto timer = std::make_unique<AsioTimer>(mIoContext, i.timerName, i.duration, mEventData); 
      mAsioTimersVect.push_back(std::move(timer));

    }

    mSocket = std::make_unique<AsioUnixSocket<T>>(mIoContext, mUnitName, mEventData); 
  }

  void sendMsg(std::unique_ptr<T> d, std::string unitName) {
    mSocket->sendMsg(std::move(d), unitName);
  }

  T getSocketData() {
    return *(mSocket->getSocketData());
  }

  msg getEventData() {
    return *(mEventData);
  }

  int waitForEvents() {
    return mIoContext.run_one();
  }

private:
  std::unique_ptr<AsioUnixSocket<T>> mSocket;
  boost::asio::io_context mIoContext;
  std::string mUnitName;
  std::vector<std::unique_ptr<AsioTimer>> mAsioTimersVect;
  std::shared_ptr<msg> mEventData;
};


}

#endif