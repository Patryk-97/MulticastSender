#include "MulticastSender.h"

MulticastSender::MulticastSender()
{
   this->ipProtocol = IpProtocol::IPV4;
   this->socketId = INVALID_SOCKET;
}

MulticastSender::~MulticastSender()
{
   this->reset();
}

void MulticastSender::reset(void)
{
   this->close();
   this->socketId = INVALID_SOCKET;
}

bool MulticastSender::init(IpProtocol ipProtocol)
{
   // locals
   int family = AF_INET;
   int type = SOCK_DGRAM;
   int proto = IPPROTO_UDP;
   bool rV = true;
   sockaddr_in localSocketAddr;
   int localSocketAddrSize = sizeof(localSocketAddr);

   if (ipProtocol == IpProtocol::IPV6)
   {
      family = AF_INET6;
      this->ipProtocol = IpProtocol::IPV6;
   }

   this->socketId = ::socket(family, type, proto);
   if (this->socketId == INVALID_SOCKET)
   {
      rV = false;
   }

   return rV;
}

bool MulticastSender::setMulticastSettings(const char* address, const uint16_t port)
{
   // locals
   bool rV = true;

   memset((char*)&groupSock, 0, sizeof(groupSock));

   this->fillIpProtocolFamily(&groupSock);
   this->fillPort(port, &groupSock);
   rV = this->fillNetworkAddressStructure(address, &groupSock);

   if (rV == true)
   {
      this->disableLoopback();
   }

   if (rV == true)
   {
      this->localInterfaceConfig();
   }

   return rV; 
}

bool MulticastSender::disableLoopback(void)
{
   // locals
   bool rV = true;
   char loopch = 0;

   if (::setsockopt(this->socketId, IPPROTO_IP, IP_MULTICAST_LOOP,
      (char*)&loopch, sizeof(loopch)) < 0)
   {
      rV = false;
   }

   return false;
}

bool MulticastSender::localInterfaceConfig(void)
{
   // locals
   bool rV = true;
   char myname[32] = { 0 };

   gethostname(myname, 32);

   rV = this->fillNetworkAddressStructure(myname, (sockaddr_in*)&localInterface);

   if (rV == true && ::setsockopt(this->socketId, IPPROTO_IP, IP_MULTICAST_IF,
      (char*)&localInterface, sizeof(localInterface)) < 0)
   {
      rV = false;
   }

   return rV;
}

bool MulticastSender::sendTo(const std::string& sendBuff, int& bytesSend)
{
   // locals
   bool rV = true;

   bytesSend = ::sendto(this->socketId, sendBuff.c_str(), sendBuff.length() + 1, 0,
      (sockaddr*)&this->groupSock, sizeof(this->groupSock));

   if (bytesSend == SOCKET_ERROR)
   {
      rV = false;
   }

   return rV;
}

void MulticastSender::close()
{
   if (this->socketId != INVALID_SOCKET)
   {
      ::closesocket(this->socketId);
   }
}

void MulticastSender::fillAddrInfoCriteria(addrinfo* hints) const
{
   memset(hints, 0, sizeof(*hints));
   hints->ai_socktype = SOCK_DGRAM;
   hints->ai_family = this->ipProtocol == IpProtocol::IPV4 ? AF_INET : AF_INET6;
}

bool MulticastSender::fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr)
{
   // locals
   bool rV = true;
   std::unique_ptr<addrinfo> hints = std::make_unique<addrinfo>();
   addrinfo* res = nullptr;

   if (address == nullptr)
   {
      socketAddr->sin_addr.s_addr = INADDR_ANY;
   }
   else if (address[0] >= '0' && address[0] <= '9')
   {
      if (::inet_pton(AF_INET, address, &socketAddr->sin_addr) != 1)
      {
         rV = false;
      }
   }
   else
   {
      this->fillAddrInfoCriteria(hints.get());
      if (0 != ::getaddrinfo(address, nullptr, hints.get(), &res))
      {
         rV = false;
      }
      else
      {
         socketAddr->sin_addr.S_un = ((sockaddr_in*)(res->ai_addr))->sin_addr.S_un;
         ::freeaddrinfo(res);
      }
   }

   return rV;
}

void MulticastSender::fillPort(uint16_t port, sockaddr_in* socketAddr)
{
   socketAddr->sin_port = htons(port);
}

void MulticastSender::fillIpProtocolFamily(sockaddr_in* socketAddr)
{
   if (this->ipProtocol == IpProtocol::IPV4)
   {
      socketAddr->sin_family = AF_INET;
   }
   else
   {
      socketAddr->sin_family = AF_INET6;
   }
}