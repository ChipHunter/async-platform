#include <iostream>
#include <iomanip>
#include <map>

#include <mqtt_client_cpp.hpp>
#include <boost/asio.hpp>

void func(MQTT_NS::error_code ec) {
      std::cout << "async_disconnect callback: " << ec.message() << std::endl;
}

class my_mqtt_client {
public:
  my_mqtt_client() {
    MQTT_NS::setup_log();
    c = MQTT_NS::make_async_client(ioc, "test.mosquitto.org", "1883");
    using packet_id_t = typename std::remove_reference_t<decltype(*c)>::packet_id_t;
    int count = 0;
    auto disconnect = [&] {
        if (++count == 5) {
            c->async_disconnect(&func);
        }
    };

    // Setup client
    c->set_client_id("cid1");
    c->set_clean_session(true);

    // Setup handlers
    c->set_connack_handler(
        [&]
        (bool sp, MQTT_NS::connect_return_code connack_return_code){
            std::cout << "Connack handler called" << std::endl;
            std::cout << "Session Present: " << std::boolalpha << sp << std::endl;
            std::cout << "Connack Return Code: "
                      << MQTT_NS::connect_return_code_to_str(connack_return_code) << std::endl;
            if (connack_return_code == MQTT_NS::connect_return_code::accepted) {
                pid_sub1 = c->acquire_unique_packet_id();
                c->async_subscribe(
                    pid_sub1,
                    "$SYS/broker/clients/total",
                    MQTT_NS::qos::at_most_once,
                    // [optional] checking async_subscribe completion code
                    []
                    (MQTT_NS::error_code ec){
                        std::cout << "async_subscribe callback: " << ec.message() << std::endl;
                    }
                );
            }
            return true;
        });

    c->set_publish_handler(
        [&]
        (MQTT_NS::optional<packet_id_t> packet_id,
         MQTT_NS::publish_options pubopts,
         MQTT_NS::buffer topic_name,
         MQTT_NS::buffer contents){
            std::cout << "publish received."
                      << " dup: "    << pubopts.get_dup()
                      << " qos: "    << pubopts.get_qos()
                      << " retain: " << pubopts.get_retain() << std::endl;
            if (packet_id)
                std::cout << "packet_id: " << *packet_id << std::endl;
            std::cout << "topic_name: " << topic_name << std::endl;
            std::cout << "contents: " << contents << std::endl;
            disconnect();
            return true;
        });

    // Connect
    c->async_connect(
        // [optional] checking underlying layer completion code
        []
        (MQTT_NS::error_code ec){
            std::cout << "async_connect callback: " << ec.message() << std::endl;
        }
    );
  }

  void runIt() {
    ioc.run();
  }
private:
  std::shared_ptr<MQTT_NS::callable_overlay<MQTT_NS::async_client<MQTT_NS::tcp_endpoint<as::ip::tcp::socket, as::io_context::strand>>>> c;
  std::uint16_t pid_sub1;
  boost::asio::io_context ioc;
};


int main(int argc, char** argv) {
  my_mqtt_client cli;

  cli.runIt();

}
