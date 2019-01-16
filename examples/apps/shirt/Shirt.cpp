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
#include "UdpHandler.h"
#include "openthread-system.h"

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

//
// This function is called when the node's Thread role has changed
//
void Shirt::ThreadStateChangedCallback(uint32_t flags, void *context)
{
    otDeviceRole currentRole = otThreadGetDeviceRole(reinterpret_cast<otInstance *>(context));
    switch (currentRole)
    {
    case OT_DEVICE_ROLE_CHILD:
    case OT_DEVICE_ROLE_ROUTER:
        Gpio::SetRgbLed(LED2_G);
        break;
    default:
        break;
    }
}

//
// This function is called when the Join operation completes
//
void Shirt::JoinCompleteCallback(otError error, void *context)
{
    otInstance *instance = reinterpret_cast<otInstance *>(context);

    switch (error) {
        case OT_ERROR_SECURITY:
        case OT_ERROR_NOT_FOUND:
        case OT_ERROR_RESPONSE_TIMEOUT:
        //TODO: handle
        Gpio::SetRgbLed(LED2_R);
        break;
        case OT_ERROR_NONE:
            error = otThreadSetEnabled(instance, true);
            assert(OT_ERROR_NONE == error);
            otSetStateChangedCallback(instance, ThreadStateChangedCallback, instance);
        break;
        default:
        break;
    }
}

//
// This function is called when BUTTON_1 has been pressed
//
void ButtonPressHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    shirt->SendSignal();
    Gpio::SetRgbLed(LED2_B);
}

void Shirt::SendSignal()
{
    mUdpHandler->SendToggle(UDPPORT);
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
                            PACKAGE_VERSION, NULL, JoinCompleteCallback, mInstance);
    assert(OT_ERROR_NONE == error);

    return error;
}

//
// This function is the main application loop.
//
void Shirt::Run()
{
    otError     error;

    error       = InitThread();
    assert(OT_ERROR_NONE == error);

    mUdpHandler = new UdpHandler(mInstance);
    mUdpHandler->Open(UDPPORT);  // udp open / bind

    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(mInstance);
        otSysProcessDrivers(mInstance);
    }

    otInstanceFinalize(mInstance);
    delete mUdpHandler;
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
