//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

enum class eventName {TIMER};

struct msg {
  eventName event;
  std::string name;
};

class AsioTimer {
  public:
    AsioTimer(boost::asio::io_context& io, std::string name, std::chrono::milliseconds duration, std::shared_ptr<msg> m) : mTimerMsg(m), mTimerName(name) {
      mTimer = std::make_shared<boost::asio::steady_timer>(io, duration);
      mTimer->async_wait(boost::bind(&AsioTimer::timerHandler, this,
            boost::asio::placeholders::error, mTimer, mTimerMsg, mTimerName));

    }

  private:
    std::shared_ptr<boost::asio::steady_timer> mTimer;
    std::shared_ptr<msg> mTimerMsg;
    std::string mTimerName;

    void timerHandler(const boost::system::error_code& /*e*/,
    std::shared_ptr<boost::asio::steady_timer> t, std::shared_ptr<msg> m, std::string timerName) {
      m->event = eventName::TIMER;
      m->name = timerName;
      t->expires_at(t->expiry() + boost::asio::chrono::milliseconds(1000));
      t->async_wait(boost::bind(&AsioTimer::timerHandler, this,
            boost::asio::placeholders::error, t, m, mTimerName));
    }
};

int main() {
  boost::asio::io_context io;

  std::shared_ptr<msg> m = std::make_shared<msg>();
  auto duration = std::chrono::milliseconds(1000);

  AsioTimer timer(io, "asio_timer", duration,  m);

  while (io.run_one()) {

    if (m->event == eventName::TIMER)
      std::cout << "timer is: " << m->name<< std::endl;
    else
      std::cout << "unknown timer" << std::endl;
  }

  return 0;
}
