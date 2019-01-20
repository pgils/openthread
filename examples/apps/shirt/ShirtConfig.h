#ifndef SHIRTCONFIG_H
#define SHIRTCONFIG_H

#include "gbrxml.h"
#include <openthread/instance.h>
#include <string>

#define     JOINERID    "D0M001"
#define     UDPPORT     8012

#define     SHIRT_INITALIZED 2

enum class NodeRole
{
    ACTUATOR        = 1,
    SENSOR          = 2,
};

enum class SignalAction
{
    NONE    = 1,
    OFF     = 2,
    RED     = 3,
    GREEN   = 4,
    BLUE    = 5,
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
    bool isSensor();
    bool isActuator();

    void SetNodeConfig(NodeConfig *config);
    std::string * GetSignalXML();
    std::string * GetNodeConfigXML();

    void ReceiveSignal(Signal *signal);

private:
    otInstance  *mInstance;
    NodeConfig  mConfig;

    bool GetRole(const NodeRole role);
    template <typename T>
    std::string * GetXML(T *input);
};

#endif // SHIRTCONFIG_H
