/*

#include "UdpTransport.h"

using namespace Pure;


UdpTransport::UdpTransport(std::string ip_string, int port)
{
  g_type_init();

  GInetAddress* address = g_inet_address_new_from_string(ip_string.c_str());
  targetAddress = g_inet_socket_address_new(address, port);
}



UdpTransport::~UdpTransport()
{
  // todo: cleanup
}


int UdpTransport::request(char* data, int size, char* buffer)
{
  GError* err = NULL;
  GSocket* socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
  
  GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
  GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 21334);
  g_socket_bind(socket, socketAddress, true, &err);
  g_object_unref(inetAddress);
  g_object_unref(socketAddress);

  if(err)
  {
    std::cerr << "ERROR: couldn't open a udp socket." << std::endl;
  }

  int result = request(socket, err, data, size, buffer);

  g_object_unref(socket);

  return result;
}//end request

int UdpTransport::request(GSocket* socket, GError* err, char* data, int size, char* buffer)
{
  g_socket_set_blocking(socket, false);
  int result = g_socket_send_to(socket, targetAddress, data, size, NULL, &err);
  
  g_socket_set_blocking(socket, true);
  result = g_socket_receive(socket, buffer, 255, NULL, NULL);

  return result;
}//end request


void UdpTransport::handle_udp_message(char* data, int size)
{
  if(data[0] == 0xFF) //notification
  {
    int pos = 1;
    UInt16 target = read<UInt16>(data,pos);
    UInt64 timestamp = read<UInt64>(data,pos);
    char* message = &(data[pos]);
    int msg_size = size - pos;
    assert(msg_size >= 0);

  }
  else //request
  {
    int pos = 0;
    Byte id = read<Byte>(data,pos);
    Pure::Action action = (Pure::Action)read<Byte>(data,pos);
    UInt16 target = read<UInt16>(data,pos);
    Byte result = read<Byte>(data,pos);
    char* message = &(data[pos]);
    int msg_size = size - pos;
    assert(msg_size >= 0);

  }
}//end handle_udp_message
*/