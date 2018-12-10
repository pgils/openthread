/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements a simple CLI for the CoAP service.
 */

#include "iot_udp.hpp"

#include <openthread/thread.h>
#include <openthread/message.h>
#include <openthread/udp.h>

#include "cli/cli.hpp"
#include "common/encoding.hpp"

using ot::Encoding::BigEndian::HostSwap16;

namespace ot {
namespace Cli {

IotUdp::IotUdp(otInstance *instance)
{
    this->instance = instance;
    memset(&mSocket, 0, sizeof(mSocket));
}

otError IotUdp::IotUdpBind()
{
    otError    error;
    otSockAddr sockaddr;
    long       value = 1212;

    memset(&sockaddr, 0, sizeof(sockaddr));

    error = otIp6AddressFromString("::", &sockaddr.mAddress);
    SuccessOrExit(error);

    sockaddr.mPort    = static_cast<uint16_t>(value);
    sockaddr.mScopeId = OT_NETIF_INTERFACE_ID_THREAD;

    error = otUdpBind(&mSocket, &sockaddr);

exit:
    return error;
}

otError IotUdp::IotUdpClose()
{
    return otUdpClose(&mSocket);
}

otError IotUdp::ProcessOpen()
{
    return otUdpOpen(instance, &mSocket, HandleUdpReceive, this);
}

otError IotUdp::IotUdpSend(char *argv)
{
    otError       error;
    otMessageInfo messageInfo;
    otMessage *   message = NULL;
    int           curArg  = 0;

    memset(&messageInfo, 0, sizeof(messageInfo));

    long value = 1212;

    error = otIp6AddressFromString("ff03::1", &messageInfo.mPeerAddr);
    SuccessOrExit(error);

    messageInfo.mPeerPort    = static_cast<uint16_t>(value);
    messageInfo.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;

    message = otUdpNewMessage(instance, NULL);
    VerifyOrExit(message != NULL, error = OT_ERROR_NO_BUFS);

    error = otMessageAppend(message, argv, static_cast<uint16_t>(strlen(argv)));
    SuccessOrExit(error);

    error = otUdpSend(&mSocket, message, &messageInfo);

exit:

    if (error != OT_ERROR_NONE && message != NULL)
    {
        otMessageFree(message);
    }

    return error;
}


void IotUdp::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<UdpExample *>(aContext)->HandleUdpReceive(aMessage, aMessageInfo);
}

void IotUdp::HandleUdpReceive(otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    uint8_t buf[1500];
    int     length;

    // mInterpreter.mServer->OutputFormat("%d bytes from ", otMessageGetLength(aMessage) - otMessageGetOffset(aMessage));
    // mInterpreter.mServer->OutputFormat(
    //     "%x:%x:%x:%x:%x:%x:%x:%x %d ", HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[0]),
    //     HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[1]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[2]),
    //     HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[3]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[4]),
    //     HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[5]), HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[6]),
    //     HostSwap16(aMessageInfo->mPeerAddr.mFields.m16[7]), aMessageInfo->mPeerPort);
    //
    // length      = otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, sizeof(buf) - 1);
    // buf[length] = '\0';
    //
    // mInterpreter.mServer->OutputFormat("%s\r\n", buf);
}

} // namespace Cli
} // namespace ot
