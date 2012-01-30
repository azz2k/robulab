/*
class NotificationHandler
{
public:
  virtual void handle_message(char* data, int size) = 0;
};


class NotificationSender
{
public:
  virtual int get_message(char* data) = 0;
};


class NotificationSenderManager
{
public:
  NotificationSenderManager(NotificationSender& sender) 
    : sender(sender)
  {
    GError* err = NULL;
    socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

    if(err)
    {
      std::cerr << "ERROR: couldn't open a udp socket." << std::endl;
    }
  }

  void send(GSocketAddress* targetAddress)
  {
    int size = sender.get_message(buffer);

    GError* err = NULL;
    g_socket_set_blocking(socket, false);
    int result = g_socket_send_to(socket, targetAddress, buffer, size, NULL, &err);
  }//end send


private:
  GSocket* socket;
  NotificationSender& sender;
  char buffer[1024];
};


class NotificationHandlerManager
{
public:
  NotificationHandlerManager(NotificationHandler& handler) 
    : handler(handler)
  {
    GError* err = NULL;
    socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);

    GInetAddress* inetAddress = g_inet_address_new_any(G_SOCKET_FAMILY_IPV4);
    GSocketAddress* socketAddress = g_inet_socket_address_new(inetAddress, 60000);
    g_socket_bind(socket, socketAddress, true, &err);
    g_object_unref(inetAddress);
    g_object_unref(socketAddress);

    if(err)
    {
      std::cerr << "ERROR: couldn't open a udp socket." << std::endl;
    }
  }

  void receive()
  {
    GError* err = NULL;
    g_socket_set_blocking(socket, true);
    g_socket_set_timeout(socket, 1);
    int size = g_socket_receive(socket, buffer, 1024, NULL, NULL);

    handler.handle_message(buffer, size);
  }//end send



  // HACK
  GSocket* socket;

  private:
  NotificationHandler& handler;
  char buffer[1024];
};


class UdpTransport
{
public:
  UdpTransport(std::string ip_string, int port);
  ~UdpTransport();


  int request(char* data, int size, char* buffer);


  void register_notificationSender(NotificationSender& sender)
  {
    NotificationSenderManager* m = new NotificationSenderManager(sender);
    senderList.push_back(m);
  }//end register_notificationSender


  void register_notificationHandler(NotificationHandler& handler, char id1, char id2, char interval)
  {
    NotificationHandlerManager* m = new NotificationHandlerManager(handler);

    int size = 7;
    char data[7] = { 0x07, Pure::INSERT, 0x01, 0x00, id1, id2, interval };
    char buffer[255];

    GError* err = NULL;
    int result = request(m->socket, err, data, size, buffer);
  
    
    if(!err && result > 3)// && buffer[4] == 0x00)
    {
      handlerList.push_back(m);
    }

    while(true)
    {
      send_receive();
    }//end while
  }//end register_notificationHandler

  void unregister_notificationHandler(char id1, char id2, char interval)
  {
    int size = 6;
    char data[6] = { 0x03, Pure::_DELETE, 0x01, 0x00, id1, id2 };
    char buffer[255];

    GError* err = NULL;
    int result = request(data, size, buffer);
  }//end unregister_notificationHandler


  void send_receive()
  {
    std::list<NotificationSenderManager*>::iterator iter_sender;
    for(iter_sender = senderList.begin(); iter_sender != senderList.end(); ++iter_sender)
    {
      (*iter_sender)->send(targetAddress);
    }//end for


    std::list<NotificationHandlerManager*>::iterator iter_handler;
    for(iter_handler = handlerList.begin(); iter_handler != handlerList.end(); ++iter_handler)
    {
      (*iter_handler)->receive();
    }//end for
  }//end send_receive


  void loop(int cycle)
  {
    while(true)
    {
      send_receive();

      // wait some time
      #ifdef WIN32
      Sleep(cycle);
      #else
      usleep(cycle * 1000);
      #endif
    }//end while
  }//end loop


private:

  std::list<NotificationHandlerManager*> handlerList;
  std::list<NotificationSenderManager*> senderList;

  //GSocket* my_socket;
  GSocketAddress* targetAddress;


  void handle_udp_message(char* data, int size);

  int request(GSocket* socket, GError* err, char* data, int size, char* buffer);

  
  template < typename T >
  T read(char* data, int& pos)
  {
    T res = *((T*)(&data[pos]));
    pos += sizeof(T);
    return res;
  }//end read


  template < typename T >
  int read(T& res, char* data, int& pos)
  {
    res = *((T*)(&data[pos]));
    pos += sizeof(T);
    return pos;
  }//end read
  */