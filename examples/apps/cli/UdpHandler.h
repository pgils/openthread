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

class UdpHandler
{
public:
    explicit UdpHandler(otInstance *instance);
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

    // This function multicasts the "toggleled" command to the network.
    //
    // @param[in]   port        Target port for the command.
    otError SendToggle(uint16_t port);

private:
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(otMessage *aMessage, const otMessageInfo *aMessageInfo);

    otUdpSocket *mSocket;
    otInstance * mInstance;
};

#endif /* UDPHANDLER_H_ */
