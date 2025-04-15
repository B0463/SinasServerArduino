#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iomanip>
#include <cstdint>

using namespace std;

void help();

int configTTY(struct termios *tty, int arduinoStream);

int main(int argc, char* argv[]) {
    if(argc < 3) {
        help();
        return 0;
    }
    char* portName = argv[1];
    int arduinoStream = open(portName, O_RDWR | O_NOCTTY);

    if(arduinoStream == -1) {
        cerr << "Can't open \"" << portName << "\" port.\n";
        return 0;
    } 

    struct termios tty;
    if(configTTY(&tty, arduinoStream) < 0) {
        cerr << "Error setting serial port.\n";
    }

    char dataToSend = stoi(argv[2], nullptr, 16);

    ssize_t bytesWritten = write(arduinoStream, &dataToSend, 1);

    if(bytesWritten < 0) {
        cerr << "Error sending data to \"" << portName << "\" port.\n";
    }

    cout << "\033[1;36m<<\033[0m 0x" << hex << setw(2) << setfill('0') << (dataToSend & 0xFF) << "\n";

    uint8_t packageSize;
    uint8_t byteReceived;

    while(1) {
        ssize_t bytesReads = read(arduinoStream, &packageSize, 1);
        
        if(bytesReads > 0) {
            cout << "\033[0;32m>>\033[0m 0x" << hex << setw(2) << setfill('0') << (packageSize & 0xFF) << "\n";
            break;
        }
        if(bytesReads < 0) {
            cerr << "\033[0;31m>>\033[0m " << "\033[1;31mERROR\033[0m\nError: " << strerror(errno) << ".\n";
        }
    }

    for(int i=0;i<packageSize;i++) {
        ssize_t bytesReads = read(arduinoStream, &byteReceived, 1);

        if(bytesReads > 0) {
            cout << "\033[0;36m>>\033[0m 0x" << hex << setw(2) << setfill('0') << (byteReceived & 0xFF) << "\n";
        }
        if(bytesReads < 0) {
            cerr << "\033[0;31m>>\033[0m " << "ERROR\nError: " << strerror(errno) << ".\n";
        }
    }

    close(arduinoStream);
}

void help() {
    cout << "Use: ./program <devide> <command>\nExemple: ./program /dev/ttyUSB0 45\n";
}

int configTTY(struct termios *tty, int arduinoStream) {
    memset(tty, 0, sizeof(*tty));
    if(tcgetattr(arduinoStream, tty) != 0) return -1;

    cfsetispeed(tty, B57600);
    cfsetospeed(tty, B57600);

    tty->c_cflag &= ~PARENB;
    tty->c_cflag &= ~CSTOPB;
    tty->c_cflag &= ~CSIZE;
    tty->c_cflag |= CS8;
    tty->c_cflag |= CREAD | CLOCAL;

    if(tcsetattr(arduinoStream, TCSANOW, tty) != 0) return -1;

    return 0;
}