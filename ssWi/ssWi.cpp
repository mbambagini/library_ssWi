/** \file ssWi.cpp
 *  \brief implementation of the internal functions for mananing the protocol
 *
 */
 
#include "ssWiChannel.hpp"
#include "ssWiPort.hpp"
#include "ssWi.hpp"

#include "mbed.h"
#include "rtos.h"

#include <map>



/** \brief first byte of the header
 *
 * The header is composed of 3 bytes
 */
#define START_0 255

/** \brief second byte of the header
 *
 * The header is composed of 3 bytes
 */
#define START_1 130

/** \brief third byte of the header
 *
 * The header is composed of 3 bytes
 */
#define START_2 255


/** 
 * dimension of the buffers used by the sender and receiver procedure
 */ 
#define INTERNAL_BUFFER_SIZE 100


/** \brief channel abstraction
 *
 * This object represent the communication channel that can be
 * different means
 */
ssWiChannel* channel = NULL;

/** \brief serialize read and write operations
 *
 * simultaneous multiple accesses to the mean are avoided
 */
Mutex mutexChannel;

/** \brief set of registered communication ports
 *
 */
std::map<int, ssWiPort> ports;

/** \brief transmission rate
 *
 */
int TXRate;

/** \brief reading rate
 *
 */
int RXRate;


void functionSender();
void functionReceiver();

static Thread readingThread;
static Thread writingThread;

bool ssWi_init (ssWiChannel* c, int rateTX, int rateRX)
{
    if (channel!=NULL)
        return false;
    //mutexChannel.lock();
    channel = c;
    //mutexChannel.unlock();
    TXRate = rateTX > 0 ? 1000/rateTX : 0;
    RXRate = rateRX > 0 ? 1000/rateRX : 0;
    
    readingThread.start(functionReceiver);
    writingThread.start(functionSender);

    return true;
}

void functionSender() {
    static DigitalOut led4(LED4);
    static char buffer[INTERNAL_BUFFER_SIZE];

    while (1) {
        led4 = !led4;
        int n = 3;
        int numFrames = 0;
        for (std::map<int, ssWiPort>::iterator it = ports.begin();
            it != ports.end(); it++) {
            if ((*it).second.isModified()) {
                buffer[n++] = (*it).first;
                PortValue tmp = (*it).second.getTXValue();
                memcpy(&buffer[n], &tmp, sizeof(PortValue));
                n += sizeof(PortValue);
                numFrames++;
            }
        }
        if (numFrames > 0) {
            buffer[0] = START_0;
            buffer[1] = START_1;
            buffer[2] = START_2;
            if (!mutexChannel.trylock())
                return;
            channel->write(buffer, n);
            mutexChannel.unlock();
        }
        thread_sleep_for(TXRate);
    }
}

void functionReceiver ()
{
    static char buffer[INTERNAL_BUFFER_SIZE];

    while(1) {
        if (!mutexChannel.trylock())
            return;
        int n = channel->read(buffer);
        mutexChannel.unlock();
        bool alreadyIn = false;
        for (int i=0; i<(n-2);) {
            bool found = false;
            if (buffer[i]==START_0 && buffer[i+1]==START_1 && buffer[i+2]==START_2) {
                found = true;
                alreadyIn = true;
                i += 3;
            }
            if ((found || alreadyIn) && (n-i)>=(sizeof(PortID)+sizeof(PortValue))) {
                PortID port = buffer[i++];
                PortValue value = 0;
                memcpy(&value, &buffer[i], sizeof(PortValue));
                i += sizeof(PortValue);
                if (ports.find(port)!=ports.end())
                    ports[port].setRXValue(value);
            }
        }
        thread_sleep_for(RXRate);
    }
}


inline bool ssWi_isActive (PortID port)
{
    return channel!=NULL && ports.find(port)!=ports.end();
}


bool ssWi_setPort (PortID port)
{
    if (channel==NULL)
        return false;
    ports[port];
    return true;
}


bool ssWi_unsetPort (PortID port)
{
    if (!ssWi_isActive(port))
        return false;
    ports.erase(port);
    return true;
}
