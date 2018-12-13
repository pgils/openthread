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
    otError Open();
    otError SendToggle();

private:
    static void HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
    void        HandleUdpReceive(otMessage *aMessage, const otMessageInfo *aMessageInfo);

    otUdpSocket *mSocket;
    otInstance * mInstance;
};

#endif /* UDPHANDLER_H_ */
