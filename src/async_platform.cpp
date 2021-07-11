#include "async_platform.h"

namespace async_platform {

AsyncPlatform::AsyncPlatform(std::string unitName, 
                              std::shared_ptr<msg> m, 
                              std::vector<timerData>& timerDataVect)  : 
                              mUnitName(unitName) { 

  for (auto i : timerDataVect) {

    auto timer = std::make_unique<AsioTimer>(mIoContext, i.timerName, i.duration, m); 
    mAsioTimersVect.push_back(std::move(timer));

  }

  mSocket = std::make_unique<AsioUnixSocket>(mIoContext, mUnitName, m); 
}

int AsyncPlatform::waitForEvents() {
  return mIoContext.run_one();
}

}