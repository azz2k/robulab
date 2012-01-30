/**

*/

#ifndef _UdpClient_h_
#define _UdpClient_h_

#include <glib.h>
#include <gio/gio.h>

#include "Tools/BasicTypes.h"
#include "Tools/Buffer.h"

namespace Pure
{

// Wraps calls to the g-sockets API
// Used by the UdpTransport class.
class UdpClient
{
public:

    UdpClient(UInt16 localPort, std::string remoteHost, UInt16 remotePort);  
    ~UdpClient();
    
    //
    bool readMessage(Byte* buffer, Int32 maxlength, Int32& length);
    //
    bool writeMessage(const Byte* buffer, Int32 length); 

    //
    bool readMessage(Buffer& buffer);
    //
    bool writeMessage(const Buffer& buffer);

private:
    
    GSocket* _socket;

    GSocketAddress* _remoteAddress;
};

}//end Pure

#endif // _UdpClient_h_