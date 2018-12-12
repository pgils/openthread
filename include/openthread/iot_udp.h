#ifndef OPENTHREAD_IOT_UDP_H_
#define OPENTHREAD_IOT_UDP_H_

#include <openthread/message.h>

#ifdef __cplusplus
extern "C" {
#endif

IotUdp(otInstance *instance);
otError IotUdpBind();
otError IotUdpClose();
otError IotUdpOpen();
otError IotUdpSend(char *argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_UDP_H_
