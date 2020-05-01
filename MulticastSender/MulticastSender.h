#pragma once

#ifndef __MULTICAST_SENDER_H__
#define __MULTICAST_SENDER_H__

#include "IpProtocol.h"

#include <Winsock2.h>
#include <string>
#include <stdint.h>
#include <memory>
#include <Ws2tcpip.h>

class MulticastSender
{
public:
   MulticastSender();
   ~MulticastSender();
   void reset(void);
   bool init(IpProtocol ipProtocol);
   bool setMulticastSettings(const char* address, const uint16_t port);
   bool disableLoopback(void);
   bool localInterfaceConfig(void);
   bool sendTo(const std::string& sendBuff, int& bytesSend);
   void close(void);

private:
   void fillAddrInfoCriteria(addrinfo* hints) const;
   bool fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr);
   void fillPort(uint16_t port, sockaddr_in* socketAddr);
   void fillIpProtocolFamily(sockaddr_in* socketAddr);

private:
   SOCKET socketId;
   sockaddr_in groupSock;
   in_addr localInterface;
   IpProtocol ipProtocol;
};

#endif // __MULTICAST_SENDER_H__