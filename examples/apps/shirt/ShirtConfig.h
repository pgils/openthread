#ifndef SHIRTCONFIG_H
#define SHIRTCONFIG_H

#include "gbrxml.h"
#include <openthread/instance.h>
#include <string>

#define     JOINERID    "D0M001"
#define     UDPPORT     8012

#define     SHIRT_INITALIZED 2

enum class SignalAction
{
    NONE    = 0,
    OFF     = 1,
    RED     = 2,
    GREEN   = 3,
    BLUE    = 4,
};

class ShirtConfig
{
public:
    ShirtConfig(otInstance *instance);
    virtual ~ShirtConfig()      = default;

private:
    ShirtConfig( const ShirtConfig& );
    ShirtConfig& operator=( const ShirtConfig& );

public:
    bool isInitialized();
    void SetNodeConfig(NodeConfig *config);
    std::string * GetSignalXML();
    std::string * GetNodeConfigXML();

    void ReceiveSignal(Signal *signal);

private:
    otInstance  *mInstance;
    NodeConfig  mConfig;

    template <typename T>
    std::string * GetXML(T *input);
};

#endif // SHIRTCONFIG_H
