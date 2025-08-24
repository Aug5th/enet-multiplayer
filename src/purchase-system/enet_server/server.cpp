#include <enet/enet.h>
#include <iostream>
#include <unordered_map>
#include <string>

struct ClientState {
    std::string id;
    int money = 100;
};

struct PeerData {
    std::string id;
};

void sendPacket(ENetPeer* peer, const std::string& msg) {
    ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

int main() {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet!\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        std::cerr << "Failed to create ENet server host!\n";
        return EXIT_FAILURE;
    }

    std::cout << "Server is running...";


    std::unordered_map<std::string, ClientState> clientsById;
    std::unordered_map<ENetPeer*, PeerData> peerMap;

    ENetEvent event;
    while (true) {
        while (enet_host_service(server, &event, 500000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "New connection. Waiting for HELLO message...\n";
                peerMap[event.peer] = PeerData{ "" };
                break;

            case ENET_EVENT_TYPE_RECEIVE: {
                std::string msg((char*)event.packet->data, event.packet->dataLength);

                if (peerMap[event.peer].id.empty() && msg.rfind("HELLO:", 0) == 0) {
                    std::string clientId = msg.substr(6);
                    peerMap[event.peer].id = clientId;

                    if (clientsById.find(clientId) == clientsById.end()) {
                        clientsById[clientId] = ClientState{ clientId, 100 };
                        std::cout << "New client registered: " << clientId << " (100 money)\n";
                    }
                    else {
                        std::cout << "Client reconnected: " << clientId
                            << " (money=" << clientsById[clientId].money << ")\n";
                    }

                    sendPacket(event.peer, "MONEY_UPDATE:" + std::to_string(clientsById[clientId].money));
                    enet_packet_destroy(event.packet);
                    break;
                }

                // Check login
                if (peerMap[event.peer].id.empty()) {
                    sendPacket(event.peer, "ERROR: Please login first with HELLO:<id>");
                    enet_packet_destroy(event.packet);
                    break;
                }

                std::string clientId = peerMap[event.peer].id;
                auto& state = clientsById[clientId];

                // Format: COMMAND|clientMoney
                std::string command;
                int clientMoney = -1;

                size_t sep = msg.find('|');
                if (sep != std::string::npos) {
                    command = msg.substr(0, sep);
                    clientMoney = std::stoi(msg.substr(sep + 1));
                }
                else {
                    command = msg;
                }

                // Check desync
                if (clientMoney != -1 && clientMoney != state.money) {
                    sendPacket(event.peer, "DESYNC DETECTED! Disconnecting.");
                    enet_peer_disconnect(event.peer, 0);
                    enet_packet_destroy(event.packet);
                    break;
                }

                if (command == "WIN_MATCH") {
                    state.money += 500;
                    std::cout << "Client " << clientId << " - win a match"
                       << " (money=" << state.money << ")\n";
                    sendPacket(event.peer, "MONEY_UPDATE:" + std::to_string(state.money));
                }
                else if (command == "BUY_ITEM:Sword") {
                    if (state.money >= 300) {
                        state.money -= 300;
                        std::cout << "Client " << clientId << " - purchase a Sword"
                            << " (money=" << state.money << ")\n";
                        sendPacket(event.peer, "PURCHASE_OK:Sword");
                    }
                    else {
                        sendPacket(event.peer, "PURCHASE_FAIL:Not enough money");
                    }
                    sendPacket(event.peer, "MONEY_UPDATE:" + std::to_string(state.money));
                }
                else if (command == "BUY_ITEM:Shield") {
                    if (state.money >= 500) {
                        state.money -= 500;
                        std::cout << "Client " << clientId << " - purchase a Shield"
                            << " (money=" << state.money << ")\n";
                        sendPacket(event.peer, "PURCHASE_OK:Shield");
                    }
                    else {
                        sendPacket(event.peer, "PURCHASE_FAIL:Not enough money");
                    }
                    sendPacket(event.peer, "MONEY_UPDATE:" + std::to_string(state.money));
                }
                else {
                    sendPacket(event.peer, "UNKNOWN_COMMAND");
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected. (ID=" << peerMap[event.peer].id << ")\n";
                peerMap.erase(event.peer);
                break;

            default:
                break;
            }
        }
    }

    enet_host_destroy(server);
    return 0;
}