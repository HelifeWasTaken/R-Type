RSERVER_PATH="./RServer/include/RServer"
RSERVER_MESSAGES_PATH="${RSERVER_PATH}/Messages"
RSERVER_CLIENT_PATH="${RSERVER_PATH}/Client"
RSERVER_SERVER_PATH="${RSERVER_PATH}/Server"
PILEAA_PATH="./PileAA/include/PileAA"
PILEAA_EXTERNAL_PATH="${PILEAA_PATH}/external"
CLIENT_PATH="./Client/include"

hpp2plantuml \
    \
    -i ${RSERVER_PATH}/async_automated_sparse_array.hpp \
    -i ${RSERVER_PATH}/async_queue.hpp \
    -i ${RSERVER_PATH}/utils.hpp \
    \
    -i ${RSERVER_CLIENT_PATH}/AClient.hpp \
    -i ${RSERVER_CLIENT_PATH}/Client.hpp \
    -i ${RSERVER_CLIENT_PATH}/TCPClient.hpp \
    -i ${RSERVER_CLIENT_PATH}/UDPClient.hpp \
    \
    -i ${RSERVER_MESSAGES_PATH}/Messages.hpp \
    -i ${RSERVER_MESSAGES_PATH}/Types.hpp \
    \
    -i ${RSERVER_SERVER_PATH}/Server.hpp \
    \
    -i ${PILEAA_PATH}/AnimatedSprite.hpp \
    -i ${PILEAA_PATH}/AnimationRegister.hpp \
    -i ${PILEAA_PATH}/App.hpp \
    -i ${PILEAA_PATH}/BaseComponents.hpp \
    -i ${PILEAA_PATH}/BatchRenderer.hpp \
    -i ${PILEAA_PATH}/Error.hpp \
    -i ${PILEAA_PATH}/InputHandler.hpp \
    -i ${PILEAA_PATH}/InputManager.hpp \
    -i ${PILEAA_PATH}/meta.hpp \
    -i ${PILEAA_PATH}/ResourceManager.hpp \
    -i ${PILEAA_PATH}/TilesetManager.hpp \
    -i ${PILEAA_PATH}/Timer.hpp \
    -i ${PILEAA_PATH}/Types.hpp \
    -i ${PILEAA_PATH}/VectorExtension.hpp \
    \
    -i ${PILEAA_EXTERNAL_PATH}/Galbar/InputHandler.hpp \
    \
    -i ${PILEAA_EXTERNAL_PATH}/HelifeWasTaken/Silva \
    -i ${PILEAA_EXTERNAL_PATH}/HelifeWasTaken/SilvaState \
    \
    -i ${PILEAA_PAA_COMMANDS_PATH}/paa_command_ecs.hpp \
    -i ${PILEAA_PAA_COMMANDS_PATH}/paa_command_main.hpp \
    -i ${PILEAA_PAA_COMMANDS_PATH}/paa_command_state.hpp \
    -i ${PILEAA_PAA_COMMANDS_PATH}/paa_getters.hpp \
    -i ${PILEAA_PAA_COMMANDS_PATH}/paa_utilities.hpp \
    \
    -i ${CLIENT_PATH}/ClientWrapper.hpp \
    \
    -o UML
