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


#include <cassert>
#include <cstring>  // memset
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <nrfx_gpiote.h>

#include "openthread-system.h"
#include "UdpHandler.h"
#include "Gpio.h"


UdpHandler *udpHandler = NULL;

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

static void ThreadStateChangedCallback(uint32_t flags, void * p_context)
{
    otDeviceRole currentRole = otThreadGetDeviceRole(
        reinterpret_cast<otInstance*>(p_context));
    switch(currentRole)
    {
      case OT_DEVICE_ROLE_CHILD:
        Gpio::SetRgbLed(LED2_R);
        break;
      case OT_DEVICE_ROLE_ROUTER:
        Gpio::SetRgbLed(LED2_G);
        break;
      case OT_DEVICE_ROLE_LEADER:
        Gpio::SetRgbLed(LED2_B);
        break;
      default:
        break;
    }
}

void buttonPressHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (NULL != udpHandler)
    {
        udpHandler->SendToggle();
    }
}

int main(int argc, char *argv[])
{
    otError error = OT_ERROR_NONE;

    Gpio::InitLeds();
    Gpio::InitButton(&buttonPressHandler);

    while (true)
    {
        otSysInit(argc, argv);

        otInstance *instance;
        instance = otInstanceInitSingle();
        assert(instance);

        udpHandler = new UdpHandler(instance);

        otThreadSetEnabled(instance, false);  // disable thread to set configs

        error = otLinkSetChannel(instance, static_cast<uint8_t>(11));
        assert(OT_ERROR_NONE == error);

        error = otLinkSetPanId(instance, static_cast<otPanId>(0x5678));
        assert(OT_ERROR_NONE == error);

        otLinkModeConfig mode;
        memset(&mode, 0, sizeof(mode));

        mode.mRxOnWhenIdle       = true;
        mode.mSecureDataRequests = true;
        mode.mDeviceType         = true;
        mode.mNetworkData        = true;

        error = otThreadSetLinkMode(instance, mode);
        assert(OT_ERROR_NONE == error);

        otThreadSetChildTimeout(instance, static_cast<uint32_t>(10));

        error = otIp6SetEnabled(instance, true);        // ifconfig up
        assert(OT_ERROR_NONE == error);
        error = otThreadSetEnabled(instance, true);
        assert(OT_ERROR_NONE == error);

        udpHandler->Open();                        // udp open / bind

        otSetStateChangedCallback(instance,
            ThreadStateChangedCallback, instance);

        while (!otSysPseudoResetWasRequested())
        {
            otTaskletsProcess(instance);
            otSysProcessDrivers(instance);
        }

        otInstanceFinalize(instance);
        delete udpHandler;
    }

    return 0;
}
