#include "ShirtConfig.h"
#include "Gpio.h"
#include <openthread/platform/radio.h>
#include <openthread/link.h>
#include <algorithm>
#include <cstdio>


ShirtConfig::ShirtConfig(otInstance *instance)
{
    otExtAddress    addrEui64;
    char            eui64Buf[(OT_EXT_ADDRESS_SIZE*2)+1]; // 2 chars per byte + \0

    mInstance           = instance;

    otLinkGetFactoryAssignedIeeeEui64(mInstance, &addrEui64);
    sprintf(eui64Buf, "%x%x%x%x%x%x%x%x", addrEui64.m8[0], addrEui64.m8[1], addrEui64.m8[2], addrEui64.m8[3],
                                          addrEui64.m8[4], addrEui64.m8[5], addrEui64.m8[6], addrEui64.m8[7]);
    mConfig.eui64       = std::string(eui64Buf);
    mConfig.active      = 1;
    mConfig.status      = 0;
    mConfig.groups      = {0};
    mConfig.role        = 0;
    mConfig.signal      = 0;
}

void ShirtConfig::SetNodeConfig(NodeConfig *config)
{
    if(config->eui64 == mConfig.eui64)
    {
        mConfig.status  = config->status;
        mConfig.role    = config->role;
        mConfig.signal  = config->signal;
        mConfig.groups  = config->groups;
    }
}

template <typename T>
std::string * ShirtConfig::GetXML(T *input)
{
    gbrXML xmlGenerator(input);
    return xmlGenerator.GetXML();
}

std::string * ShirtConfig::GetSignalXML()
{
    Signal signal;
    signal.groups   = mConfig.groups;
    signal.signal   = mConfig.signal;

    return GetXML(&signal);
}

std::string * ShirtConfig::GetNodeConfigXML()
{
    return GetXML(&mConfig);
}

void ShirtConfig::ReceiveSignal(Signal *signal)
{
    for( int group : mConfig.groups )
    {
        if( std::find(signal->groups.begin(), signal->groups.end(), group) != signal->groups.end())
        {
            switch(static_cast<SignalAction>(signal->signal))
            {
                case SignalAction::OFF:
                Gpio::SetRgbLed(LED2_OFF);
                break;
                case SignalAction::RED:
                Gpio::SetRgbLed(LED2_R);
                break;
                case SignalAction::GREEN:
                Gpio::SetRgbLed(LED2_G);
                break;
                case SignalAction::BLUE:
                Gpio::SetRgbLed(LED2_B);
                break;
                default:
                break;
            }
            return;
        }
    }
}
