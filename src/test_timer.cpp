//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>

#include "async_types.h"

using boost::asio::local::datagram_protocol;

class AsioTimer {
  public:
    AsioTimer(boost::asio::io_context& io, std::string name, std::chrono::milliseconds duration, std::shared_ptr<msg> m) : mTimerMsg(m), mTimerName(name) {
      mTimer = std::make_shared<boost::asio::steady_timer>(io, duration);
      mTimer->async_wait(boost::bind(&AsioTimer::timerHandler, this,
            boost::asio::placeholders::error, mTimer, mTimerMsg, mTimerName));

    }

  private:
    std::shared_ptr<boost::asio::steady_timer> mTimer;
    std::shared_ptr<msg> mTimerMsg;
    std::string mTimerName;

    void timerHandler(const boost::system::error_code& /*e*/,
    std::shared_ptr<boost::asio::steady_timer> t, std::shared_ptr<msg> m, std::string timerName) {
      m->event = eventName::TIMER;
      m->name = timerName;
      t->expires_at(t->expiry() + boost::asio::chrono::milliseconds(1000));
      t->async_wait(boost::bind(&AsioTimer::timerHandler, this,
            boost::asio::placeholders::error, t, m, mTimerName));
    }
};


class AsioUnixSocket {
public:
  AsioUnixSocket(boost::asio::io_context& io, std::string name, std::shared_ptr<msg> m) : mSocketName(name) , mSocketMsg(m) {
    ::unlink(mSocketName.c_str());
    mpSocket = std::make_unique<datagram_protocol::socket>(io, datagram_protocol::endpoint(mSocketName));
    start_receive();
  }

  /* void sendMsg(std::unique_ptr<msg> m, datagram_protocol::endpoint address) { */
  /*   char buf[sizeof(m)]; */
  /*   msg* mm = m.release(); */
  /*   memcpy(buf, &mm, sizeof(m)); */
  /*   try { */
  /*     mpSocket->send_to(boost::asio::buffer(buf), address); */
  /*   } catch (std::exception& e) { */
  /*     delete mm; */
  /*     std::cerr << e.what() << std::endl; */
  /*   } */
  /* } */


private:
  void start_receive()
  {
    mpSocket->async_receive_from(
        boost::asio::buffer(recv_buffer_), mServerAddress,
        std::bind(&AsioUnixSocket::handle_receive, this,
          std::placeholders::_1,
          std::placeholders::_2,
          mSocketMsg, mSocketName));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t, std::shared_ptr<msg> mm, std::string name)
  {
    if (!error)
    {
      std::cout << "handle receive!" << std::endl;
      msg* m;
      memcpy(&m, recv_buffer_, sizeof(m));
      std::unique_ptr<msg> mmm(m);
      if (mmm->type == msgType::TIME_REQUEST) {
        std::cout << "inside" << std::endl;
        mm->event = eventName::UNIX_SOCKET;
        mm->name = name;
      }

      start_receive();
    }
  }


  private:
    std::unique_ptr<datagram_protocol::socket> mpSocket;
    datagram_protocol::endpoint mServerAddress;
    char recv_buffer_[sizeof(msg)];
    std::string mSocketName;
    std::shared_ptr<msg> mSocketMsg;
};
