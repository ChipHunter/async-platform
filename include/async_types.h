#ifndef ASYNC_PLATFORM_TYPES_H
#define ASYNC_PLATFORM_TYPES_H

#include<string>

enum class eventName {TIMER, UNIX_SOCKET};
enum class msgType { TIME_REQUEST, TIME_REPLY };

struct msg {
  eventName event;
  std::string name;
  msgType type;
};

#endif