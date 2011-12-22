
#include "UdpClient.h"

using namespace Pure;


UdpClient::UdpClient(UInt16 localPort, std::string remoteHost, UInt16 remotePort)
{
  g_type_init();

  GError* err = NULL;
  _socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

	if(err){ g_warning("%s", err->message); }

  // bind the socket
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 60000);
  g_socket_bind(_socket, socketAddress, true, &err);
  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

	if(err){ g_warning("%s", err->message); }

  //
  GInetAddress* address = g_inet_address_new_from_string(remoteHost.c_str());
  _remoteAddress = g_inet_socket_address_new(address, remotePort);
  g_object_unref(address);
}//end constructor


UdpClient::~UdpClient()
{
  g_object_unref(_remoteAddress);
  g_object_unref(_socket);
}

bool UdpClient::readMessage(Byte* buffer, Int32 maxlength, Int32& length)
{
  GError* err = NULL;
  g_socket_set_blocking(_socket, true);
  length = g_socket_receive(_socket, (char*)buffer, maxlength, NULL, &err);

  if(err){ g_warning("%s", err->message); }
  return !err;
}//end ReadMessage

bool UdpClient::writeMessage(const Byte* buffer, Int32 length)
{        
  GError* err = NULL;
  g_socket_set_blocking(_socket, false);
  int result = g_socket_send_to(_socket, _remoteAddress, (char*)buffer, length, NULL, &err);

  if(err){ g_warning("%s", err->message); }
  return !err;
}//end WriteMessage


bool UdpClient::readMessage(Buffer& buffer)
{
  Int32 length = 0;
  Buffer::Modifier modifier(buffer);
  bool result = readMessage(*modifier, buffer.MaxSize, length);
  modifier.Reset(length);
  return result;
}//end readMessage

bool UdpClient::writeMessage(const Buffer& buffer)
{
  return writeMessage(*buffer, buffer.Size());
}//end writeMessage
