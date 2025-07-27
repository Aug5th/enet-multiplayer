#include <enet/enet.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printMenu() {
    std::cout << "=== Choose Your Move ===\n";
    std::cout << "1: Scissors\n";
    std::cout << "2: Paper\n";
    std::cout << "3: Rock\n";
    std::cout << "Enter your choice (1-3): ";
}

int main() {
    if (enet_initialize() != 0) {
        std::cerr << "ENet init failed.\n";
        return 1;
    }

    ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "Client create failed.\n";
        return 1;
    }

    ENetAddress address;
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "Connection failed.\n";
        return 1;
    }

    std::cout << "Connecting to server...\n";

    bool gameStarted = false;
    bool waiting = true;

    while (true) {
        ENetEvent event;
        while (enet_host_service(client, &event, 100) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Connected to server.\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE: {
                std::string msg(reinterpret_cast<char*>(event.packet->data));
                std::cout << "[Server] " << msg << "\n";

                if (msg == "START") {
                    gameStarted = true;
                    waiting = false;
                }
                else if (msg.rfind("RESULT:", 0) == 0) {
                    std::string result = msg.substr(7);
                    std::cout << "\nGame Result: " << result << "\n\n";
                    waiting = true; // restart waiting
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server.\n";
                enet_host_destroy(client);
                enet_deinitialize();
                return 0;

            default:
                break;
            }
        }

        // Wait until both clients are connected
        if (!gameStarted || waiting) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Game started: show menu and send input
        int choice = 0;
        while (choice < 1 || choice > 3) {
            printMenu();
            std::cin >> choice;
        }

        std::string msg = "CHOICE:" + std::to_string(choice);
        ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        enet_host_flush(client);

        waiting = true;
    }

    enet_host_destroy(client);
    enet_deinitialize();
    return 0;
}