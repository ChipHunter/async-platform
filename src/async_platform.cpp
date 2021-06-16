#include "async_platform.h"

namespace async_platform {

AsyncPlatform::AsyncPlatform(std::shared_ptr<msg> m) { 
  auto duration = std::chrono::milliseconds(1000);
  mTimer = std::make_unique<AsioTimer>(mIoContext, "timer_the_timer", duration, m); 
}


int AsyncPlatform::waitForEvents() {
  return mIoContext.run_one();
}
}