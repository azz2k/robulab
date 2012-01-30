/**

*/

#ifndef _DirectoryClient_h_
#define _DirectoryClient_h_

#include "UdpTransport.h"

class DirectoryClient
{
private:

  Pure::UdpTransport& server;

  
public:
  DirectoryClient(Pure::UdpTransport& server, Pure::UInt16 instance = 0x00)
    : 
    server(server)
  {

    // request the list of all registered services
    Pure::Request request;
    request.Action = Pure::Request::Get;
    request.Target = instance;

    if(server.request(request))
    {
      if(request.response.Result == Pure::Response::OK)
      {
        Pure::UInt16 instanceNr; 
        Pure::UInt16 serviceType;

        Pure::Request requestA;
        requestA.Action = Pure::Request::Query;
        requestA.Target = instance;

        while(request.response.Data.Size() > 0)
        {
          request.response.Data >> serviceType;
          request.response.Data >> instanceNr;
          
          requestA.Data << instanceNr;
          if(server.request(requestA))
          {
            requestA.response.Data << (Pure::Byte)0;
            printf("[%d] %#x", instanceNr, serviceType);
            std::cout << ((char*)(*(requestA.response.Data))) << std::endl;
          }

        }//end while
        std::cout << std::endl;
      }
    }//end if
  }

  ~DirectoryClient()
  {
  }
};

#endif // _DirectoryClient_h_
