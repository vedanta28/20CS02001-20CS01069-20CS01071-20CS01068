#include <bits/stdc++.h>
#include <signal.h>
#include <sys/file.h>
#include "includes/const.hpp"
#include "includes/comms.hpp"
#include "includes/connector.hpp"
#include <unistd.h>

using namespace std;

std::mutex broadcastMtx;
std::mutex mtx; // Mutex for critical section in sender and receiver threads
/* NOTE: If we don't use Mutex, then clock++ like operations which are not atomic might give weird results */

vector<comms> connections;
int PORT_SERVER, PORT_CLIENT;
priority_queue <pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // Min Priority Queue

class processor {
    public: 
    int id, clock, noOfReplies;
    bool hasRequestedCritialSection, isAccessingCriticalSection, needsToRelease;
    void broadcast (string msgType) {
        lock_guard<mutex> guard(broadcastMtx);
        if (msgType == "REQUEST") {
            connections[0].sendMsg(msgType + "," + to_string(clock) + "," + to_string(id));
            connections[1].sendMsg(msgType + "," + to_string(clock) + "," + to_string(id));
        } else { // RELEASE
            connections[0].sendMsg(msgType + "," + to_string(clock));
            connections[1].sendMsg(msgType + "," + to_string(clock));
        }
    }
};

processor p;

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;
    while (getline (ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

void *sender(void *arg) {
    while(1) {
        sleep(rand() % 10); // Randomly Sleep for sometime

        mtx.lock(); // Lock the mutex before accessing the shared variables

        // Requesting Critical Section
        if (!p.hasRequestedCritialSection && !p.isAccessingCriticalSection) { // Not Accessing Critical Section or not requested it yet.
            p.clock++;
            pq.push({p.clock, p.id});
            cout << YEL << "[" << p.clock << "] " << "REQUEST CRITICAL SECTION" << "\n" << NRM; 
            p.broadcast("REQUEST"); // Request for accessing critical section.
            p.hasRequestedCritialSection = true;
        }
        if (p.needsToRelease) {
            p.clock++;
            cout << RED << "[" << p.clock << "] " << "LEFT CRITICAL SECTION" << "\n" << NRM; 
            p.broadcast("RELEASE");
            pq.pop();
            p.isAccessingCriticalSection = false;
            p.hasRequestedCritialSection = false;
            p.needsToRelease = false;
        }

        mtx.unlock(); // Unlock the mutex after accessing the shared variables
    }
}

void tryToAccessCritialSection() {
    if (p.noOfReplies == 2 && !pq.empty() && pq.top().second == p.id) {
        p.isAccessingCriticalSection = true;
        cout << GRN << "[" << p.clock << "] " << "ACCESSING CRITICAL SECTION" << "\n" << NRM; 
        sleep(rand() % 2); // Random sleep to simulate processing of the critical section
        p.noOfReplies = 0;
        p.needsToRelease = true;
    }
}

void *receiver(void *arg) {
    while(1) {
        string response = connections[*((int *)arg)].receive();
        auto result = split(response, ',');

        mtx.lock(); // Lock the mutex before accessing the shared variables

        if (result[0] == "REPLY") { // REPLY is Received
            p.clock = max(p.clock, stoi(result[1])) + 1;
            p.noOfReplies++;
            cout << BLU << "[" << p.clock << "] " << response << "\n" << NRM; 
            tryToAccessCritialSection();
        } else if (result[0] == "REQUEST") {
            p.clock = max(p.clock, stoi(result[1])) + 1;
            cout << MAG << "[" << p.clock << "] " << response << "\n" << NRM; 
            pq.push({stoi(result[1]), stoi(result[2])});
            p.clock++;
            connections[*((int *)arg)].sendMsg(string("REPLY") + "," + to_string(p.clock));
            cout << "[" << p.clock << "] " << "REPLY SEND TO PROCESSOR " << result[2] << "\n" << NRM; 
        } else { // RELEASE message
            p.clock = max(p.clock, stoi(result[1])) + 1;
            cout << CYN << "[" << p.clock << "] " << response << "\n" << NRM; 
            pq.pop();
            tryToAccessCritialSection();
        }

        mtx.unlock(); // Unlock the mutex after accessing the shared variables
    }
}

int main(int argc, char** argv) {
    srand(time(0));
    string ip_address = string(argv[1]);
    PORT_SERVER = stoi(argv[2]);
    PORT_CLIENT = stoi(argv[3]);

    // Processor Initialisation
    p.id = (PORT_CLIENT + PORT_SERVER) % 3;
    p.clock = 0;
    p.noOfReplies = 0;
    p.isAccessingCriticalSection = false;
    p.needsToRelease = false;
    p.hasRequestedCritialSection = false;

    // Connections
    connector server(1, PORT_SERVER);
    server.listenForClients();
    sleep(5); // Useful to avoid deadlock situations. Provided you run all three fast enough.
    connector client(0, PORT_CLIENT, ip_address);
    
    connections.push_back(comms(client.connectToServer()));
    connections.push_back(comms(server.acceptNow()));
    cout << GRN << "Connections Established at Processor " << p.id << "\n" << NRM;
    cout << "--------------------------------------------\n";
    cout << "[TIME] [MESSAGE]\n";
    // All the connections made at this point
    
    pthread_t thread_id[3];
    int id1 = 0, id2 = 1;
    pthread_create(&thread_id[0], NULL, sender, NULL);
    pthread_create(&thread_id[1], NULL, receiver, &id1);
    pthread_create(&thread_id[2], NULL, receiver, &id2);

    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);
    pthread_join(thread_id[2], NULL);
}
