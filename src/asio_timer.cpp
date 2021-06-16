#include "asio_timer.h"

namespace async_platform {

AsioTimer::AsioTimer(boost::asio::io_context& io, std::string name, std::chrono::milliseconds duration, std::shared_ptr<msg> m) : mTimerMsg(m), mTimerName(name) {
  mTimer = std::make_shared<boost::asio::steady_timer>(io, duration);
  mTimer->async_wait(std::bind(&AsioTimer::timerHandler, this, std::placeholders::_1, mTimer, mTimerMsg, mTimerName));
}

void AsioTimer::timerHandler(const boost::system::error_code& /*e*/,std::shared_ptr<boost::asio::steady_timer> t, std::shared_ptr<msg> m, std::string timerName) {
  m->event = eventName::TIMER;
  m->name = timerName;
  t->expires_at(t->expiry() + boost::asio::chrono::milliseconds(1000));
  t->async_wait(std::bind(&AsioTimer::timerHandler, this,
        std::placeholders::_1, t, m, mTimerName));
}

} //namespace async_plat