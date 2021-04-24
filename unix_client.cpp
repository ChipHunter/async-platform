#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <memory>
#include <thread>
#include <asio.hpp>

using asio::local::datagram_protocol;

class UnixClient {
  public:
    UnixClient(std::string socketAddr) : mSocketAddr(socketAddr), mEndpoint("/tmp/unix_socket") {

      asio::io_context io_context;

      ::unlink(mSocketAddr.c_str());
      mpSocket = std::make_unique<datagram_protocol::socket>(io_context, datagram_protocol::endpoint(mSocketAddr));

    }

    void sendMsg(std::array<char, 128>& sendBuf) {

      mpSocket->send_to(asio::buffer(sendBuf), mEndpoint);

    }

    size_t recvMsg(std::array<char, 128>& recvBuf) {

      datagram_protocol::endpoint sender_endpoint;
      return mpSocket->receive_from(
          asio::buffer(recvBuf), sender_endpoint);

    }

  private:
    std::string mSocketAddr;
    datagram_protocol::endpoint mEndpoint;
    std::unique_ptr<datagram_protocol::socket> mpSocket;

};

void client_func() {

  try {

    UnixClient client("/tmp/unix_client");
    std::array<char, 128> sendBuf = {0};
    client.sendMsg(sendBuf);

    std::array<char, 128> recv_buf;
    size_t len = client.recvMsg(recv_buf);
    std::cout.write(recv_buf.data(), len);

  } catch (std::exception& e) {

    std::cerr << e.what() << std::endl;

  }

}

int main() {

  client_func();

  return 0;

}
