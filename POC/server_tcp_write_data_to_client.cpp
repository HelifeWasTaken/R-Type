#include "poc.hpp"
#include "Network/Server.hpp"

void event_check(rtype::net::ServerEvent& event, rtype::net::Server& s)
{
    tcp_buffer_t buffer("hello world\n");

    std::cout << "event coucou" << std::endl;
    if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_CONNECTION) {
        std::cout << "Connection: " << event.get_event<size_t>() << std::endl;
        while (true) {
            s.write_tcp_socket(0, buffer, 12);
			rtype::net::ServerEvent event;
			while (s.poll(event)) {
				if (event.get_type() == rtype::net::ServerEvent::ServerEventType::TCP_DECONNECTION) {
						exit(0);
				}
			}
		}
	}
}

void poc_server_write_tcp_example(void)
{
    rtype::net::Server s(atoi(av[1]), atoi(av[2]));

    while (s.is_running()) {
        rtype::net::ServerEvent event;
        while (s.poll(event))
            event_check(event, s);
    }
    return 0;
}
