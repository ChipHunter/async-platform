#ifndef ASYNC_PLATFORM_TYPES_H
#define ASYNC_PLATFORM_TYPES_H

#include<string>

enum class eventName {NONE, TIMER, UNIX_SOCKET, MQTT};
enum class msgType { TIME_REQUEST, TIME_REPLY };
enum class mqttTypes { CONNACK , CONTENT };

struct msg {
  eventName event;
  std::string name;
  msgType type;
  mqttTypes mqttType;
};

#endif