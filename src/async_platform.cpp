//  #include "async_platform.h"

// namespace async_platform {

// template <class T>
// AsyncPlatform<T>::AsyncPlatform(std::string unitName, 
//                               std::vector<timerData>& timerDataVect)  : 
//                               mUnitName(unitName) { 

//   mEventData = std::make_shared<msg>();

//   for (auto i : timerDataVect) {

//     auto timer = std::make_unique<AsioTimer>(mIoContext, i.timerName, i.duration, mEventData); 
//     mAsioTimersVect.push_back(std::move(timer));

//   }

//   mSocket = std::make_unique<AsioUnixSocket<T>>(mIoContext, mUnitName, mEventData); 
// }

// template <class T>
// int AsyncPlatform<T>::waitForEvents() {
//   return mIoContext.run_one();
// }

// }