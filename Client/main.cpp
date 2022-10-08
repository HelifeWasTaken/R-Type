#include "PileAA/AnimatedSprite.hpp"
#include "PileAA/App.hpp"
#include "RServer/Client/Client.hpp"
#include "ClientWrapper.hpp"

#include <unordered_map>

using namespace rtype::game;

PAA_SCENE(mystate) {

    RTYPE_CLIENT client;

    PAA_START(mystate) {
        PAA_ENTITY e = PAA_NEW_ENTITY();
        PAA_SET_SPRITE(e, "spaceship");
        PAA_GET_COMPONENT(e, paa::Sprite).useAnimation("idle");

        client.run("../Client.conf");
    }

    std::unordered_map<int, PAA_ENTITY> entities;

    PAA_UPDATE {
        if (client.restart_if_necessary() == false)
            return;

        shared_message_t polled_msg;

        while (client.client->tcp().poll(polled_msg)) {
            spdlog::info("Received message from server of code({})", (int)polled_msg->code());
            switch (polled_msg->code()) {
                case message_code::CONNECTION:
                    {
                        ConnectionMessage& conn_msg = get_message<ConnectionMessage>(polled_msg.get());

                        PAA_ENTITY e = PAA_NEW_ENTITY();
                        auto& sp = PAA_SET_SPRITE(e, "spaceship");
                        sp.setPosition(rand() % 200, rand() % 200);
                        sp.useAnimation("idle");
                        entities[conn_msg.playerId()] = e;
                        spdlog::info("New player connected with id {}", conn_msg.playerId());
                        break;
                    }
                case message_code::DISCONNECTION:
                    {
                        DisconnectMessage& disconn_msg = get_message<DisconnectMessage>(polled_msg.get());

                        PAA_DESTROY_ENTITY(entities[disconn_msg.playerId()]);
                        entities.erase(disconn_msg.playerId());
                        spdlog::info("Player {} disconnected", disconn_msg.playerId());
                        break;
                    }
                default:
                    break;
            }
        }
        while (client.client->udp().poll(polled_msg)) {
            spdlog::info("Message received code: {}", (int)polled_msg->code());
        }
    }
};

PAA_PROGRAM_START(mystate, "../Resources.conf", true);
