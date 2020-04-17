#include "mbed.h"

#include "xbee.hpp"
#include "ssWiSocket.hpp"

#define TARGET_ID               1

int readValue = 0;
int writtenValue = 0;

void code_state(int);

Thread readingThread;
Thread writingThread;

void readingFunction();
void writingFunction();

//Serial pc(USBTX, USBRX, "pc", 9600);

#define UPDATE_RATE_MS    1000

int main()
{

    //pc.printf("\n\r************* CONFIG *************\n\r");

code_state(0);

    //radio module
    XBeeModule xbee(p9, p10, 102, 14);
    //XBeeAddress addr = xbee.getLocalAddress();

code_state(1);

    xbee.setDstAddress(XBeeBroadcastAddress());

code_state(2);

   //XBeeAddress addr2(0, 0);
   //xbee.getDstAddress(addr2);
   //xbee.getChannel();
   //xbee.getPanID();

    //wireless network
    xbee.init(5, 10);

    srand(time(0));

code_state(4);

    thread_sleep_for(UPDATE_RATE_MS);

    //thread
    readingThread.start(readingFunction);
    writingThread.start(writingFunction);

    while (true) {
        //code_state(writtenValue);
        //thread_sleep_for(UPDATE_RATE_MS);
        code_state(readValue);
        thread_sleep_for(UPDATE_RATE_MS);
    }

    return 0;
}

#define READ_FUNCTION_MS 1000
#define WRITE_FUNCTION_MS 3000

#define PORT_ID_TARGET_1   2
#define PORT_ID_TARGET_2   10

void readingFunction()
{
    PortID id = TARGET_ID == 1 ? PORT_ID_TARGET_1 : PORT_ID_TARGET_2;
    ssWiSocket* s = ssWiSocket::createSocket(id);

    while (1) {
        readValue = s->read();
        thread_sleep_for(READ_FUNCTION_MS);
    }
}

void writingFunction() {
    PortID id = TARGET_ID == 1 ? PORT_ID_TARGET_2 : PORT_ID_TARGET_1;
    ssWiSocket* s = ssWiSocket::createSocket(id);

    writtenValue = TARGET_ID == 1 ? 3 : 0;
    
    while (1) {
        s->write(writtenValue++);
        thread_sleep_for(WRITE_FUNCTION_MS);
  }
}

void code_state(int val) {
    static DigitalOut led1(LED1);
    static DigitalOut led2(LED2);
    static DigitalOut led3(LED3);

    led1 = val % 2;
    led2 = (val >> 1) % 2;
    led3 = (val >> 2) % 2;
}
