#include "async_platform.h"

namespace async_platform {

AsyncPlatform::AsyncPlatform(std::string unitName, 
                              std::vector<timerData>& timerDataVect)  : 
                              mUnitName(unitName) { 

  mEventData = std::make_shared<msg>();

  for (auto i : timerDataVect) {

    auto timer = std::make_unique<AsioTimer>(mIoContext, i.timerName, i.duration, mEventData); 
    mAsioTimersVect.push_back(std::move(timer));

  }

  mSocket = std::make_unique<AsioUnixSocket>(mIoContext, mUnitName, mEventData); 
}

int AsyncPlatform::waitForEvents() {
  return mIoContext.run_one();
}

}