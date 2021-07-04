#ifndef ASYNC_PLATFORM_ASIO_TIMER_H
#define ASYNC_PLATFORM_ASIO_TIMER_H

#include <string>
#include <memory>
#include <chrono>
#include <iostream>

#include <boost/asio.hpp>

#include "async_types.h"

namespace async_platform {

class AsioTimer {
public:
  AsioTimer(boost::asio::io_context& io, 
            std::string name, std::chrono::milliseconds duration, std::shared_ptr<msg> m);

private:
  std::shared_ptr<boost::asio::steady_timer> mTimer;
  std::shared_ptr<msg> mTimerMsg;
  std::string mTimerName;
  std::chrono::milliseconds mTimerDuration;

private:
  void timerHandler(const boost::system::error_code& /*e*/, 
                    std::shared_ptr<boost::asio::steady_timer> t, 
                    std::shared_ptr<msg> m, 
                    std::string timerName, 
                    std::chrono::milliseconds duration); 
};
}

#endif