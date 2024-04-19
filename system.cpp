#include <bits/stdc++.h>
#include <signal.h>
#include <sys/file.h>
#include "includes/const.hpp"
#include "includes/comms.hpp"
#include "includes/connector.hpp"
#include <unistd.h>

using namespace std;

vector<comms> connections;

int PORT_SERVER, PORT_CLIENT;

class processor {
    public: 
    int id, clock;
    void broadcast (string msgType) {
        if (msgType == "REQUEST") {
            connections[0].sendMsg(to_string(clock) + ", P" + to_string(id));
            connections[1].sendMsg(to_string(clock) + ", P" + to_string(id));
        } else { // RELEASE
            connections[0].sendMsg(msgType);
            connections[1].sendMsg(msgType);
        }
    }
};

processor p;

void *sender(void *arg) {
    while(1) {
        string msg;
        cin >> msg;
        p.broadcast(msg);
    }
}

void *receiver1(void *arg) {
    while(1) {
        cout << connections[0].receive() << endl;
    }
}

void *receiver2(void *arg) {
    while(1) {
        cout << connections[1].receive() << endl;
    }
}

int main(int argc, char** argv) {
    string ip_address = string(argv[1]);
    PORT_SERVER = stoi(argv[2]);
    PORT_CLIENT = stoi(argv[3]);

    // Processor Initialisation
    p.id = (PORT_CLIENT + PORT_SERVER) % 3;
    p.clock = 0;

    // Connections
    connector server(1, PORT_SERVER);
    server.listenForClients();
    sleep(5); // Useful to avoid deadlock situations. Provided you run all three fast enough.
    connector client(0, PORT_CLIENT, ip_address);
    
    connections.push_back(comms(client.connectToServer()));
    connections.push_back(comms(server.acceptNow()));
    cout << GRN << "Connections Established\n" << NRM;
    // All the connections made at this point
    
    pthread_t thread_id[3];
    pthread_create(&thread_id[0], NULL, sender, NULL);
    pthread_create(&thread_id[1], NULL, receiver1, NULL);
    pthread_create(&thread_id[2], NULL, receiver2, NULL);

    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);
    pthread_join(thread_id[2], NULL);
}
