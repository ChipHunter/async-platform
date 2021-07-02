#include "async_platform.h"

namespace async_platform {

AsyncPlatform::AsyncPlatform(std::string unitName, std::shared_ptr<msg> m)  : mUnitName(unitName) { 
  auto duration = std::chrono::milliseconds(1000);
  mTimer = std::make_unique<AsioTimer>(mIoContext, "timer_the_timer", duration, m); 
  mSocket = std::make_unique<AsioUnixSocket>(mIoContext, "/tmp/asio_unix_socket", m); 
}

int AsyncPlatform::waitForEvents() {
  return mIoContext.run_one();
}

}