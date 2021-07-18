//#include "asio_unix_socket.h"

// namespace async_platform {

// template <class T>
// AsioUnixSocket<T>::AsioUnixSocket(boost::asio::io_context& io, 
//                                   std::string name, 
//                                   std::shared_ptr<msg> m) : 
//                                   mSocketName(name), 
//                                   mSocketPath("/tmp/unix_socket_" + mSocketName), 
//                                   mSocketMsg(m) {
//   ::unlink(mSocketPath.c_str());
//   mpSocket = std::make_unique<boost::asio::local::datagram_protocol::socket>(io, 
//                           boost::asio::local::datagram_protocol::endpoint(mSocketPath));
//   start_receive();
// }

// template <class T>
// void AsioUnixSocket<T>::start_receive() {
//   mpSocket->async_receive_from(
//       boost::asio::buffer(recv_buffer_), mServerAddress,
//       std::bind(&AsioUnixSocket::handle_receive, this,
//         std::placeholders::_1,
//         std::placeholders::_2,
//         mSocketMsg, mSocketName));
// }

// template <class T>
// void AsioUnixSocket<T>::handle_receive(const boost::system::error_code& error,
//     std::size_t, std::shared_ptr<msg> mm, std::string name)
// {
//   if (!error) {
//     T* m;
//     memcpy(&m, recv_buffer_, sizeof(m));

//     mm->type = eventType::UNIX_SOCKET;
//     mm->eventName = name;
//     mSocketData.reset(m);

//     start_receive();
//   }
// }

// template <class T>
// void AsioUnixSocket<T>::sendMsg(std::unique_ptr<T> d, std::string unitName) {
//   boost::asio::local::datagram_protocol::endpoint address("/tmp/unix_socket_" + unitName);
//   char buf[sizeof(d)];
//   T* mm = d.release();
//   memcpy(buf, &mm, sizeof(d));
//   try {
//     mpSocket->send_to(boost::asio::buffer(buf), address);
//   } catch (std::exception& e) {
//     delete mm;
//     std::cerr << e.what() << std::endl;
//   }
// }

//} //namespace async_platform