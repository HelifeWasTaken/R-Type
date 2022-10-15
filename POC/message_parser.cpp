#include "poc.hpp"
#include "RServer/Messages/Messages.hpp"
#include "RServer/Messages/Types.hpp"
#include <iostream>

void poc_message_parser()
{
    /*
    // sid may be the entity or component id, or any other id,
    // here 42 is used just for example
    rtype::net::vector2i v(12.34f, 56.78f);
    rtype::net::SyncMessage sync_msg(42, v);

    assert(sync_msg.code() == rtype::net::message_code::SYNC_MSG);
    assert(sync_msg.sid() == 42);
    assert(sync_msg.type() == rtype::net::serializable_type::VECTOR2I);
    assert(sync_msg.get<rtype::net::vector2f>().x == 12.34f);
    assert(sync_msg.get<rtype::net::vector2f>().y == 56.78f);

    std::vector<Byte> buffer = sync_msg.serialize();
    std::cout << "serialized length: " << buffer.size() << std::endl;

    boost::shared_ptr<rtype::net::SyncMessage> sync_msg2 = rtype::net::Message::deserialize<rtype::net::SyncMessage>(buffer);
    assert(sync_msg2->code() == rtype::net::message_code::SYNC_MSG);
    assert(sync_msg2->sid() == 42);
    assert(sync_msg2->type() == rtype::net::serializable_type::VECTOR2I);
    assert(sync_msg2->get<rtype::net::vector2i>().x == 12.34f);
    assert(sync_msg2->get<rtype::net::vector2i>().y == 56.78f);

    boost::shared_ptr<rtype::net::IMessage> msg = rtype::net::parse_message(buffer);
    assert(msg->code() == rtype::net::message_code::SYNC_MSG);

    boost::shared_ptr<rtype::net::SyncMessage> msg2 = rtype::net::parse_message<rtype::net::SyncMessage>(buffer);
    assert(msg2->code() == rtype::net::message_code::SYNC_MSG);

    boost::shared_ptr<rtype::net::UpdateMessage> msg3 = rtype::net::parse_message<rtype::net::UpdateMessage>(buffer);
    assert(msg3 == nullptr);

    std::cout << "poc_message_parser: OK" << std::endl;
    */
}
