#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

int number_of_players = 3;

void send_coordinates_to_all(std::vector<sf::UdpSocket> &sending_sockets, std::vector<int> &players_ports,
                             std::vector<float> &players_data) {
    sf::IpAddress recipient = sf::IpAddress::LocalHost;
    std::vector<sf::Packet> packets(number_of_players);
    int c = -1;
    for (int i = 0; i < number_of_players; ++i) {
        packets[i] << c;
    }
    for (int i = 0; i < number_of_players * 4; ++i) {
        for (int j = 0; j < number_of_players; ++j) {
            if (i / 4 == j)continue;
            packets[j] << players_data[i];
        }
    }
    for (int i = 0; i < number_of_players; ++i) {
        sending_sockets[i].send(packets[i], recipient, players_ports[i]);
    }
}

void
send_player_event_to_all(std::vector<sf::UdpSocket> &sending_sockets,
                         std::vector<int> &players_ports, int player_index,
                         sf::Event event) {
    sf::IpAddress recipient = sf::IpAddress::LocalHost; // In future we need a list of address
    sf::Packet packet;
    int c = -2;
    packet << c;
    packet << player_index;
    packet << event.type;
    packet << event.key.code;

    for (int i = 0; i < number_of_players; ++i) {
        if (i+1 == player_index)continue;
        sending_sockets[i].send(packet, recipient, players_ports[i]);
    }
}

int main() {

    sf::UdpSocket receive_socket;
    std::vector<sf::UdpSocket> sending_sockets(number_of_players); // one sending socket for each player
    receive_socket.setBlocking(false);
    for (int i = 0; i < number_of_players; ++i) {
        sending_sockets[i].setBlocking(false);
    }
    std::vector<int> players_ports{54000, 53999,53998};
    int server_port = 54002;
    receive_socket.bind(server_port);

    sf::Packet packet;
    sf::IpAddress sender;
    unsigned short port;

    std::vector<float> players_data(number_of_players * 4);// four numbers for each player
    int just_received = 0;
    while (true) {

        int c = -2;
        int key_code = -1;
        int player_index = -1;
        if (receive_socket.receive(packet, sender, port) == sf::Socket::Done) {
            packet >> player_index;
            packet >> c;
            sf::Event event;

            float x1 = 0;
            float y1 = 0;
            float vx = 0;
            float vy = 0;

            if (c == sf::Event::EventType::KeyPressed || c == sf::Event::EventType::KeyReleased) {

                packet >> key_code;
                event.type = sf::Event::EventType(c);
                event.key.code = sf::Keyboard::Key(key_code);
                send_player_event_to_all(sending_sockets, players_ports, player_index, event);

            } else if (c == -1) {
                just_received++;
                packet >> x1;
                packet >> y1;
                packet >> vx;
                packet >> vy;
                players_data[4 * (player_index - 1) + 0] = x1;
                players_data[4 * (player_index - 1) + 1] = y1;
                players_data[4 * (player_index - 1) + 2] = vx;
                players_data[4 * (player_index - 1) + 3] = vy;

            }
        }
        if (just_received == number_of_players) {
            just_received = 0;
            send_coordinates_to_all(sending_sockets, players_ports, players_data);
        }

    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
