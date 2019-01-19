/*
 * UdpHandler.h
 *
 *  Created on: Dec 11, 2018
 *      Author: pollop
 */

#ifndef UDPHANDLER_H_
#define UDPHANDLER_H_

#include "openthread-core-config.h"

#include <openthread/udp.h>

class otInstance;
class gbrXML;

class UdpHandler
{
public:
    UdpHandler(otInstance *instance, void (*messageCallback)(gbrXML*, void *context), void *context);
    virtual ~UdpHandler();

private:
    UdpHandler(const UdpHandler &);
    UdpHandler &operator=(UdpHandler const &);

public:
    // This function opens a network socket that listens on all
    // network addresses for UDP messages.
    //
    // @param[in]   port        Port to listen on.
    otError Open(uint16_t port);

    // This function multicasts a message to the network.
    //
    // @param[in]   port        Target port for the command.
    // @param[in]   messageStr  Message to be sent.
    otError SendMulticast(uint16_t port, const char *messageStr);

private:
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(otMessage *aMessage, const otMessageInfo *aMessageInfo);

    void        (*mMessageCallback)(gbrXML*, void *context);
    void        *mCallbackContext;

    otUdpSocket *mSocket;
    otInstance  *mInstance;
};

#endif /* UDPHANDLER_H_ */
