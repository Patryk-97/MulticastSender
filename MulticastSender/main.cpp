#include "MulticastSender.h"
#include "WinsockManager.h"

#include <iostream>
#include <memory>

#define DLL_WINSOCK_VERSION MAKEWORD(2, 2)

int main()
{
   std::unique_ptr<WinsockManager> winsockManager = std::make_unique<WinsockManager>();
   std::unique_ptr<MulticastSender> multicastSender = nullptr;
   std::string ipAddress;
   uint16_t port;
   std::string sendBuff;
   int bytesSend = 0;

   if (false == winsockManager->startup(DLL_WINSOCK_VERSION))
   {
      printf("Winsock initialization error\n");
      return -1;
   }

   std::cout << "Enter broadcast ip address: ";
   std::cin >> ipAddress;

   std::cout << "Enter port: ";
   std::cin >> port;

   multicastSender = std::make_unique<MulticastSender>();

   if (true == multicastSender->init(IpProtocol::IPV4))
   {
      std::cout << "Socket initialized\n";
   }
   else
   {
      std::cout << "Cannot initialiaze a socket\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
      winsockManager->cleanup();
      return -1;
   }

   if (multicastSender->setMulticastSettings(ipAddress.c_str(), port) == false)
   {
      std::cout << "Broadcast settings have not been set\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
      multicastSender->close();
      winsockManager->cleanup();
      return -1;
   }

   std::cout << "Enter message: ";
   std::cin >> sendBuff;

   if (multicastSender->sendTo(sendBuff, bytesSend) == false)
   {
      std::cout << "Data have not been sent\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
   }

   multicastSender->close();
   std::cout << "Socket closed" << "\n";
   winsockManager->cleanup();
   std::cin.get();
   return 0;
}