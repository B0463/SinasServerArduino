#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iomanip>
#include <cstdint>

using namespace std;

// define functions
void help();

int configTTY(struct termios *tty, int arduinoStream);

int main(int argc, char* argv[]) {
    if(argc < 3) { // check the number of arguments
        help(); // print help if not enough arguments
        return 0;
    }

    char* portName = argv[1]; // get the port name from the arguments
    int arduinoStream = open(portName, O_RDWR | O_NOCTTY); // open the tty port stream

    if(arduinoStream == -1) { // check if the port is opened
        cerr << "Can't open \"" << portName << "\" port.\n";
        return 0;
    } 

    struct termios tty; // create a termios struct to configure the tty port
    if(configTTY(&tty, arduinoStream) < 0) { // check if the port is configured
        cerr << "Error setting serial port.\n";
    }

    char dataToSend = stoi(argv[2], nullptr, 16); // convert the command to a char

    ssize_t bytesWritten = write(arduinoStream, &dataToSend, 1); // send the command to the arduino

    if(bytesWritten < 0) { // check if the command is sent
        cerr << "Error sending data to \"" << portName << "\" port.\n";
    }

    cout << "\033[1;36m<<\033[0m 0x" << hex << setw(2) << setfill('0') << (dataToSend & 0xFF) << "\n";

    uint8_t packageSize;
    uint8_t byteReceived;

    while(1) { // wait for the len response
        ssize_t bytesReads = read(arduinoStream, &packageSize, 1); // read the last byte
        
        if(bytesReads > 0) { // check if the byte is read and then close the loop
            cout << "\033[0;32m>>\033[0m 0x" << hex << setw(2) << setfill('0') << (packageSize & 0xFF) << "\n";
            break;
        }
        if(bytesReads < 0) { // if there is an error
            cerr << "\033[0;31m>>\033[0m " << "\033[1;31mERROR\033[0m\nError: " << strerror(errno) << ".\n";
        }
        // if the bytesize is 0 try again
    }

    for(int i=0;i<packageSize;i++) { // read the rest of the package 
        ssize_t bytesReads = read(arduinoStream, &byteReceived, 1); // read the byte

        if(bytesReads > 0) { // check if the byte is read
            cout << "\033[0;36m>>\033[0m 0x" << hex << setw(2) << setfill('0') << (byteReceived & 0xFF) << "\n";
        }
        if(bytesReads < 0) { // if there is an error
            cerr << "\033[0;31m>>\033[0m " << "ERROR\nError: " << strerror(errno) << ".\n";
        }
        // if the bytesize is 0 passes
    }

    close(arduinoStream); // close the port stream
    return 0;
}

void help() { // show the help message
    cout << "Use: ./program <devide> <command>\nExemple: ./program /dev/ttyUSB0 45\n";
}

int configTTY(struct termios *tty, int arduinoStream) { // configure the tty port
    memset(tty, 0, sizeof(*tty)); // clear the struct
    if(tcgetattr(arduinoStream, tty) != 0) return -1; // get the current attributes

    cfsetispeed(tty, B57600); // set the input speed
    cfsetospeed(tty, B57600); // set the output speed

    tty->c_cflag &= ~PARENB; // disable parity
    tty->c_cflag &= ~CSTOPB; // set 1 stop bit
    tty->c_cflag &= ~CSIZE; // clear the size bits
    tty->c_cflag |= CS8; // set 8 data bits
    tty->c_cflag |= CREAD | CLOCAL; // enable receiver and ignore modem control lines
    tty->c_cflag &= ~HUPCL; // disable hang up on close
    tty->c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG); // disable canonical mode, echo, and signal characters
    tty->c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // disable input flags

    if(tcsetattr(arduinoStream, TCSANOW, tty) != 0) return -1; // set the attributes

    return 0;
}