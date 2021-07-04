#include "asio_timer.h"

namespace async_platform {

AsioTimer::AsioTimer(boost::asio::io_context& io, std::string name, std::chrono::milliseconds duration, std::shared_ptr<msg> m) : mTimerMsg(m), mTimerName(name) {
  mTimer = std::make_shared<boost::asio::steady_timer>(io, duration);
  mTimer->async_wait(std::bind(&AsioTimer::timerHandler, this, std::placeholders::_1, mTimer, mTimerMsg, mTimerName, duration));
}

void AsioTimer::timerHandler(const boost::system::error_code& /*e*/,std::shared_ptr<boost::asio::steady_timer> t, std::shared_ptr<msg> m, std::string timerName, std::chrono::milliseconds duration) {
  m->type = eventType::TIMER;
  m->eventName = timerName;
  t->expires_at(t->expiry() + duration);
  t->async_wait(std::bind(&AsioTimer::timerHandler, this,
        std::placeholders::_1, t, m, mTimerName, duration));
}

} //namespace async_plat