#ifndef SHIRT_H
#define SHIRT_H

#include "ShirtConfig.h"
#include "UdpHandler.h"
#include <nrfx_gpiote.h>
#include <openthread/thread.h>

class gbrXML;

class Shirt
{
public:
    Shirt();
    virtual ~Shirt();

private:
    Shirt( const Shirt& );
    Shirt& operator=( const Shirt& );

public:
    void Run();

private:
    otInstance  *mInstance;
    UdpHandler  *mUdpHandler;
    ShirtConfig *mShirtConfig;

    otError InitThread();
    static void ThreadStateChangedCallback(uint32_t flags, void *context);
    static void JoinCompleteCallback(otError error, void *context);
    static void MessageReceivedCallback(gbrXML *xml, void *context);

    void SendNodeConfig();
    void SendHeartbeat();
};

extern Shirt *shirt;

#endif // SHIRT_H
