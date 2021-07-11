#ifndef ASYNC_PLATFORM_TYPES_H
#define ASYNC_PLATFORM_TYPES_H

#include<string>
#include<chrono>

enum class eventType {NONE, TIMER, UNIX_SOCKET};

struct socketData {
  int data;
};

struct msg {
  eventType type;
  std::string eventName;
};

struct timerData {
  std::string timerName;
  std::chrono::milliseconds duration;
};

#endif