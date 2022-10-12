#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "RServer/Client/Client.hpp"
#include "ClientWrapper.hpp"

#include <unordered_map>
#include <iostream>

using namespace rtype::game;

RTYPE_CLIENT client;

PAA_SCENE(game_scene) {

    PAA_START(game_scene) {
        PAA_ENTITY e = PAA_NEW_ENTITY();
        PAA_SET_SPRITE(e, "spaceship").useAnimation("idle");
        PAA_GET_COMPONENT(e, paa::Sprite).useAnimation("idle");
    }

};

PAA_SCENE_DECL(client_connect);

PAA_SCENE(connect_room) {

    std::string _roomToken = "";

    PAA_START(connect_room) {
        std::cout << "Enter room token: ";
        std::cin >> _roomToken;

        client.client->tcp().send(rtype::net::RequestConnectRoom(_roomToken));
    }

    PAA_UPDATE {
        if (!client.is_service_on()) {
            PAA_SET_SCENE(client_connect);
            return;
        }

        rtype::net::shared_message_t msg;
        while (client.client->tcp().poll(msg)) {
            if (msg->type() == rtype::net::message_type::CONNECT_ROOM_REQ_REP) {
                auto rep = parse_message<rtype::net::RequestConnectRoomReply>(msg.get());
                if (!rep) {
                    spdlog::error("Client: Failed to parse CONNECT_ROOM_REQ_REP message");
                    PAA_SET_SCENE(connect_room);
                    return;
                }
                if (rep->playerID() == RTYPE_INVALID_PLAYER_ID) {
                    spdlog::error("Client: Failed to connect to room");
                    PAA_SET_SCENE(connect_room);
                    return;
                } else {
                    spdlog::info("Client: Connected to room as {}", rep->playerID());
                    PAA_SET_SCENE(game_scene);
                    return;
                }
            } else {
                spdlog::info("Client: Received message of type {}", msg->type());
            }
        }
    }
};

PAA_SCENE(create_room) {

    PAA_START(create_room) {
        client.client->tcp().send(
            rtype::net::SignalMarker(rtype::net::message_code::CREATE_ROOM)
        );
    }

    PAA_UPDATE {
        rtype::net::shared_message_t msg;
        while (client.client->tcp().poll(msg)) {
            if (msg->code() == rtype::net::message_code::CREATE_ROOM_REPLY) {
                auto rep = parse_message<rtype::net::CreateRoomReply>(msg.get());
                if (!rep) {
                    spdlog::error("Client: Failed to parse CREATE_ROOM_REPLY message");
                    exit(1);
                }
                spdlog::info("Client create_room: Created room with token {}", rep->token());
                PAA_SET_SCENE(game_scene);
                return;
            } else {
                spdlog::info("Client create_room: Received message of type {}", msg->type());
            }
        }
    }
};

PAA_SCENE(client_connect) {

    paa::Timer _timer;
    std::string choice;

    PAA_START(client_connect) {
        client.run("../Client.conf");

        _timer.setTarget(1000);

        //std::cout << "host or connect: ";
        //std::cin >> choice;
    }

    PAA_UPDATE {
        if (client.is_service_on() == false)
            exit(1);

        if (client.restart_if_necessary() == false)
            return;

        rtype::net::shared_message_t msg;
        while (client.client->tcp().poll(msg));
        while (client.client->udp().poll(msg));

        if (client.client->tcp().is_connected()) {
            if (client.client->udp().is_connected()) {
                if (choice == "host")
                    PAA_SET_SCENE(create_room);
                else
                    PAA_SET_SCENE(connect_room);
            } else if (_timer.isFinished()) {
                client.client->udp().feed_request(
                    client.client->tcp().token(),
                    client.client->tcp().id());
                _timer.restart();
            }
        }
    }
};

PAA_PROGRAM_START(client_connect, "../Resources.conf", true);
