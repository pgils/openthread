#ifndef SHIRT_H
#define SHIRT_H

#include <nrfx_gpiote.h>
#include <openthread/thread.h>

#define JOINERID    "D0M001"
#define UDPPORT     8012

class UdpHandler;

class Shirt
{
public:
    Shirt()             {};
    virtual ~Shirt()    {};

private:
    Shirt( const Shirt& );
    Shirt& operator=( const Shirt& );

public:
    void Run();
    void SendSignal();

private:
    otInstance  *mInstance;
    UdpHandler  *mUdpHandler;

    otError InitThread();
    static void ThreadStateChangedCallback(uint32_t flags, void *context);
    static void JoinCompleteCallback(otError error, void *context);
};

extern Shirt *shirt;

#endif // SHIRT_H
