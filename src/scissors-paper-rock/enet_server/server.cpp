#include <enet/enet.h>
#include <iostream>
#include <string>

enum Choice {
	SCISSORS = 1,
	PAPER = 2,
	ROCK = 3
};

std::string CheckResult(Choice yourChoice, Choice oponentChoice)
{
	if (yourChoice == oponentChoice) return "DRAW";
	if ((yourChoice == SCISSORS && oponentChoice == PAPER) ||
		(yourChoice == PAPER && oponentChoice == ROCK) ||
		(yourChoice == ROCK && oponentChoice == SCISSORS))
	{
		return "WIN";
	}
	return "LOSE";
}

int main() {
	if (enet_initialize() != 0) {
		std::cerr << "ENet init failed.\n";
		return 1;
	}

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 1234;

	ENetHost* server = enet_host_create(&address /* the address to bind the server host to */
		, 2			/* allow up to 2 clients and/or outgoing connections */
		, 2			/* allow up to 2 channels to be used, 0 and 1 */
		, 0			/* assume any amount of incoming bandwidth */
		, 0);			/* assume any amount of outgoing bandwidth */

	if (!server) {
		std::cerr << "Server create failed.\n";
		return 1;
	}

	std::cout << "Server started. Waiting for players...\n";

	ENetPeer* players[2] = { nullptr, nullptr };
	int connectedPlayers = 0;

	Choice choices[2] = { static_cast<Choice>(0), static_cast<Choice>(0) };
	bool hasChosen[2] = { false, false };

	while (true) {
		ENetEvent event;
		/* Wait up to 10000 milliseconds for an event. */
		while (enet_host_service(server, &event, 10000) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
			{
				if (connectedPlayers < 2) {
					players[connectedPlayers++] = event.peer;
					std::cout << "Player " << event.peer->address.port << " connected: " << connectedPlayers << " / 2\n";

					std::string msg = "Player connected " + std::to_string(connectedPlayers) + "/2, please wait";
					ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);

					if (connectedPlayers == 2) {
						std::cout << "Both players connected. Starting game...\n";
						for (int i = 0; i < 2; ++i) {
							ENetPacket* startPacket = enet_packet_create("START", 6, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(players[i], 0, startPacket);
						}
					}
				}
				break;
			}

			case ENET_EVENT_TYPE_RECEIVE: 
			{
				std::string data(reinterpret_cast<char*>(event.packet->data));
				std::cout << "Received from player " << event.peer->address.port <<": " << data << "\n";

				if (data.rfind("CHOICE:", 0) == 0) {
					int choiceNum = std::stoi(data.substr(7));

					// Find which index this peer is
					int index = (event.peer == players[0]) ? 0 : (event.peer == players[1]) ? 1 : -1;
					if (index != -1) {
						choices[index] = static_cast<Choice>(choiceNum);
						hasChosen[index] = true;
					}
				}

				if (hasChosen[0] && hasChosen[1]) {
					Choice c1 = choices[0];
					Choice c2 = choices[1];

					std::string r1 = CheckResult(c1, c2);
					std::string r2 = CheckResult(c2, c1);

					ENetPacket* pkt1 = enet_packet_create(("RESULT:" + r1).c_str(), r1.size() + 8, ENET_PACKET_FLAG_RELIABLE);
					ENetPacket* pkt2 = enet_packet_create(("RESULT:" + r2).c_str(), r2.size() + 8, ENET_PACKET_FLAG_RELIABLE);

					enet_peer_send(players[0], 0, pkt1);
					enet_peer_send(players[1], 0, pkt2);

					std::cout << "Result sent. " << players[0]->address.port << "/" << players[1]->address.port 
						<< ": " << r1 << " / " << r2 << "\n";

					hasChosen[0] = hasChosen[1] = false;
				}

				enet_packet_destroy(event.packet);
				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				std::cout << "Player disconnected.\n";

				for (int i = 0; i < 2; ++i) {
					if (players[i] == event.peer) {
						players[i] = nullptr;
						hasChosen[i] = false;
						connectedPlayers--;
					}
				}
				break;
			}

			default:
				break;
			}
		}
	}

	enet_host_destroy(server);
	enet_deinitialize();
	return 0;
}