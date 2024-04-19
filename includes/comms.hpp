#ifndef COMMS
#define COMMS

#include <bits/stdc++.h>
#include <string.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "const.hpp"
#include <unistd.h>
#include <fcntl.h>


using namespace std;

class comms {
    int fd;
public:
    comms(int f) : fd(f) {}
    bool sendMsg(string s) {
        string sizeString = to_string(s.size());
        sizeString = string(MESSAGE_SIZE_LENGTH - sizeString.length(), '0') + sizeString;
        string paddedMessage = sizeString + s + string(MESSAGE_CONTENT_LENGTH - s.size(), 'X');
        char buf[MAX_SIZE];
        memcpy(buf, paddedMessage.c_str(), paddedMessage.size());
        int byte_total = 0, byte_now = 0, len = MAX_SIZE;
        while (byte_total < len) {
            if (!((byte_now = send(fd, &buf[byte_total], len - byte_total, 0)) != -1)) {
                perror("cannot send");
                return false;
            }
            byte_total += byte_now;
        }
        return true;
    }
    string receive() {
        char buf[MAX_SIZE] = {0};
        int byte_total = 0, byte_now = 0, len = MAX_SIZE;
        while (byte_total < len) {
            if (!((byte_now = read(fd, &buf[byte_total], len - byte_total)) != -1)) {
                perror("cannot receive");
                return "";
            }
            byte_total += byte_now;
        }
        string s(buf + MESSAGE_SIZE_LENGTH, MESSAGE_CONTENT_LENGTH);
        int sizeofMessage=stoi(string(buf,20));
        s=s.substr(0,sizeofMessage);
        return s;
    }

    void disconnect() {
        close(fd);
    }
};


#endif
