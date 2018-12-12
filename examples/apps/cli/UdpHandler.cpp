/*
 * UdpHandler.cpp
 *
 *  Created on: Dec 11, 2018
 *      Author: pollop
 */

#include "UdpHandler.h"

#include <openthread/message.h>
#include <openthread/udp.h>

#include "common/tasklet.hpp"
#include "common/timer.hpp"
#include "common/instance.hpp"

#include <hal/nrf_gpio.h>

#define LED1_G         NRF_GPIO_PIN_MAP(0,6)

UdpHandler::UdpHandler(otInstance *instance)
{
    this->mInstance = instance;
    this->mSocket   = new otUdpSocket();
}

UdpHandler::~UdpHandler()
{
    delete this->mSocket;
}

otError UdpHandler::Open()
{
  otError       error;
  otSockAddr    sockaddr;

  error = otUdpOpen(this->mInstance, this->mSocket, HandleUdpReceive, this);
  assert(OT_ERROR_NONE == error);
  error = otIp6AddressFromString("::", &sockaddr.mAddress);
  assert(OT_ERROR_NONE == error);

  sockaddr.mPort    = static_cast<uint16_t>(12121);
  sockaddr.mScopeId = OT_NETIF_INTERFACE_ID_THREAD;

  error = otUdpBind(this->mSocket, &sockaddr);
  assert(OT_ERROR_NONE == error);

  return error;
}

otError UdpHandler::SendToggle()
{
  otError       error;
  otMessageInfo messageInfo;
  otMessage     *message = NULL; // TODO: fix C++11 compat
  const char    *messageStr = "toggleled";

  memset(&messageInfo, 0, sizeof(messageInfo));

  error = otIp6AddressFromString("ff03::1", &messageInfo.mPeerAddr);
  assert(OT_ERROR_NONE == error);

  messageInfo.mPeerPort     = static_cast<uint16_t>(12121);
  messageInfo.mInterfaceId  = OT_NETIF_INTERFACE_ID_THREAD;

  message = otUdpNewMessage(this->mInstance, NULL);

  error = otMessageAppend(message, messageStr,
    static_cast<uint16_t>(strlen(messageStr)));
  assert(OT_ERROR_NONE == error);

  error = otUdpSend(this->mSocket, message, &messageInfo);
  assert(OT_ERROR_NONE == error);

  return error;
}


void UdpHandler::HandleUdpReceive(void *aContext, otMessage *aMessage,
  const otMessageInfo *aMessageInfo)
{
    static_cast<UdpHandler *>(aContext)->HandleUdpReceive(aMessage,
      aMessageInfo);
}

void UdpHandler::HandleUdpReceive(otMessage *aMessage,
  const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t buf[1500];
    int     length;

    length      = otMessageRead(aMessage, otMessageGetOffset(aMessage),
                  buf, sizeof(buf) - 1);
    buf[length] = '\0';

    if (strcmp((char*)buf, "toggleled") == 0)
    {
      nrf_gpio_pin_toggle(LED1_G);
    }
}
