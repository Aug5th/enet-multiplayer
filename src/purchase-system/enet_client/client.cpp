#include <enet/enet.h>
#include <iostream>
#include <string>

int main() {
    int localMoney = 0;

    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet!\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost* client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "Failed to create ENet client!\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "No available peers for connection.\n";
        return EXIT_FAILURE;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) 
    {
        std::cout << "Connected to server.\n";

        std::string myId;
        std::cout << "Enter your Player ID: ";
        std::getline(std::cin, myId);
        std::string helloMsg = "HELLO:" + myId;
        ENetPacket* helloPacket = enet_packet_create(helloMsg.c_str(), helloMsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, helloPacket);
    }
    else {
        std::cerr << "Failed to connect to server.\n";
        return EXIT_FAILURE;
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE) 
    {
        std::string msg((char*)event.packet->data);

        if (msg.rfind("MONEY_UPDATE:", 0) == 0) {
            localMoney = std::stoi(msg.substr(13));
            std::cout << "[Server]: Money = " << localMoney << "\n";
        }
        else {
            std::cout << "[Server]: " << msg << "\n";
        }

        enet_packet_destroy(event.packet);
    }

    bool running = true;

    while (running) {
        std::string input;
        std::cout << "Enter command (WIN / BUY:Sword / BUY:Shield / HACK / EXIT): ";
        std::getline(std::cin, input);

        std::string message;

        if (input == "EXIT") {
            running = false;
        }
        else if (input == "WIN") {
            message = "WIN_MATCH|" + std::to_string(localMoney);
        }
        else if (input == "BUY:Sword") {
            message = "BUY_ITEM:Sword|" + std::to_string(localMoney);
        }
        else if (input == "BUY:Shield") {
            message = "BUY_ITEM:Shield|" + std::to_string(localMoney);
        }
        else if (input == "HACK") {
            
            message = "BUY_ITEM:Sword|999999";
        }

        if (!message.empty()) {
            ENetPacket* packet = enet_packet_create(message.c_str(), message.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
        }

        while (enet_host_service(client, &event, 500) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                std::string msg((char*)event.packet->data);

                if (msg.rfind("MONEY_UPDATE:", 0) == 0) {
                    localMoney = std::stoi(msg.substr(13));
                    std::cout << "[Server]: Money = " << localMoney << "\n";
                }
                else {
                    std::cout << "[Server]: " << msg << "\n";
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server.\n";
                running = false;
                break;

            default:
                break;
            }
        }
    }

    enet_peer_disconnect(peer, 0);
    enet_host_flush(client);
    enet_host_destroy(client);
    return 0;
}