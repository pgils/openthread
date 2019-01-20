/*
 *  Copyright (c) 2016, The OpenThread Authors.
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

#include "Shirt.h"
#include <cassert>
#include <cstring> // memset
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/tasklet.h>
#include <openthread/joiner.h>

#include "Gpio.h"
#include "gbrxml.h"
#include "openthread-system.h"

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

//
// Constructor
//
Shirt::Shirt()
{
    otError     error;

    error       = InitThread();
    assert(OT_ERROR_NONE == error);

    mShirtConfig = new ShirtConfig(mInstance);

    mUdpHandler = new UdpHandler(mInstance, MessageReceivedCallback, this);
    assert(OT_ERROR_NONE == mUdpHandler->Open(UDPPORT));  // udp open / bind

}

Shirt::~Shirt()
{
    otInstanceFinalize(mInstance);
    delete mUdpHandler;
    delete mShirtConfig;
}

//
// This function is called when the node's Thread role has changed
//
void Shirt::ThreadStateChangedCallback(uint32_t flags, void *context)
{
    Shirt           *contextShirt   = reinterpret_cast<Shirt *>(context);
    otDeviceRole    currentRole     = otThreadGetDeviceRole(contextShirt->mInstance);
    switch (currentRole)
    {
    case OT_DEVICE_ROLE_CHILD:
    case OT_DEVICE_ROLE_ROUTER:
        Gpio::SetLed1(true);
        contextShirt->SendNodeConfig();
        break;
    default:
        Gpio::SetLed1(false);
        break;
    }
}

//
// This function is called when the Join operation completes
//
//TODO: assert if not joined
void Shirt::JoinCompleteCallback(otError error, void *context)
{
    Shirt   *contextShirt   = reinterpret_cast<Shirt *>(context);

    switch (error) {
        case OT_ERROR_SECURITY:
        case OT_ERROR_NOT_FOUND:
        case OT_ERROR_RESPONSE_TIMEOUT:
        break;
        case OT_ERROR_NONE:
            error = otThreadSetEnabled(contextShirt->mInstance, true);
            assert(OT_ERROR_NONE == error);
            otSetStateChangedCallback(contextShirt->mInstance, ThreadStateChangedCallback, contextShirt);
        break;
        default:
        break;
    }
}

//
// This function is called when a message has been received by the UDP handler.
//
void Shirt::MessageReceivedCallback(gbrXML *xml, void *context)
{
    Shirt   *contextShirt   = reinterpret_cast<Shirt*>(context);

    switch (xml->GetType())
    {
        case gbrXMLMessageType::SIGNAL:
            contextShirt->mShirtConfig->ReceiveSignal(xml->GetSignal());
            break;
        case gbrXMLMessageType::GETNODECONFIG:
            contextShirt->SendNodeConfig();
            break;
        case gbrXMLMessageType::NODECONFIG:
            contextShirt->mShirtConfig->SetNodeConfig(xml->GetNodeConfig());
            break;
        default:
        break;
    }
}

void Shirt::SendNodeConfig()
{
    otError     error;
    error = mUdpHandler->SendMulticast(UDPPORT, mShirtConfig->GetNodeConfigXML()->c_str());
    assert(OT_ERROR_NONE == error);
}

void Shirt::SendSignal()
{
    if( !mShirtConfig->isInitialized() ) { return; }

    otError     error;
    error = mUdpHandler->SendMulticast(UDPPORT, mShirtConfig->GetSignalXML()->c_str());
    assert(OT_ERROR_NONE == error);
}

//
// This function is called when BUTTON_1 has been pressed
//
void ButtonPressHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    shirt->SendSignal();
}

//
// This function configures and initializes the Thread instance
//
otError Shirt::InitThread()
{
    otError error = OT_ERROR_NONE;

    mInstance = otInstanceInitSingle();
    assert(mInstance);

    // disable thread to set configs (ignore return value!)
    otThreadSetEnabled(mInstance, false);

    // erase non-volatile network info.
    error = otInstanceErasePersistentInfo(mInstance);
    assert(OT_ERROR_NONE == error);

    otLinkModeConfig mode;
    memset(&mode, 0, sizeof(mode));

    mode.mRxOnWhenIdle       = true;
    mode.mSecureDataRequests = true;
    mode.mDeviceType         = true;
    mode.mNetworkData        = true;

    error = otThreadSetLinkMode(mInstance, mode);
    assert(OT_ERROR_NONE == error);

    error = otIp6SetEnabled(mInstance, true);  // ifconfig up
    assert(OT_ERROR_NONE == error);

    error = otJoinerStart(mInstance, JOINERID, NULL, PACKAGE_NAME, OPENTHREAD_CONFIG_PLATFORM_INFO,
                            PACKAGE_VERSION, NULL, JoinCompleteCallback, this);
    assert(OT_ERROR_NONE == error);

    return error;
}

//
// This function is the main application loop.
//
void Shirt::Run()
{

    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(mInstance);
        otSysProcessDrivers(mInstance);
    }

}

Shirt *shirt;

int main(int argc, char *argv[])
{
    Gpio::InitLeds();
    Gpio::InitButton(&ButtonPressHandler);

    while (true)
    {
        otSysInit(argc, argv);

        shirt   = new Shirt();
        shirt->Run();
        delete shirt;
    }

    return 0;
}
