
#include "UdpTransport.h"

namespace Pure
{

UdpTransport::UdpTransport(
    std::string host, 
    UInt16 remotePort, 
    UInt16 localPort) 
  :
  m_client(localPort, host, remotePort),
  m_id(0)
{
  m_lock = g_mutex_new();

  // start thread
  g_debug("Starting debug server thread");
   

  GError* err = NULL;
  connectionThread = g_thread_create(connection_thread_static, this, true, &err);
  if(err)
  {
    g_warning("Could not start connection thread: %s", err->message);
  }
}

UdpTransport::~UdpTransport()
{
  g_mutex_free(m_lock);

  if(connectionThread != NULL)
  {
    g_thread_join(connectionThread);
  }
}


void* UdpTransport::connection_thread_static(void* ref)
{
  ((UdpTransport*) ref)->mainConnection();
  return NULL;
}//end connection_thread_static


void UdpTransport::mainConnection()
{
  // receive incoming messages
  while(m_client.readMessage(read_buffer))
  {
    if(read_buffer.Size() < 1)
    {
      continue;
    }

    Byte id = read_buffer[0];

    if(0xFF == id)
    {
      if(read_buffer.Size() < 3)
      {
        continue;
      }

      
      //notification.from_message(read_buffer);

      UInt16 target = *((UInt16*)&read_buffer[1]);


      g_mutex_lock(m_lock);

      std::map<UInt16, Subscription*>::iterator iter = subscriptionMap.find(target);
      if(iter == subscriptionMap.end())
      {
        g_mutex_unlock(m_lock);
        continue;
      }

      Notification notification;
      notification.Target = target;

      for(int i = 3; i < read_buffer.Size(); i++)
      {
          notification.Data << read_buffer[i];
      }
      
      Subscription& subscription = *(iter->second);

      g_mutex_lock(subscription.m_lock);
      subscription.notifications.push(notification);
      g_mutex_unlock(subscription.m_lock);

      g_mutex_unlock(m_lock);

    }
    else
    {
      // ... It's a response to a request.
      if(read_buffer.Size() < 5)
      {
        continue;
      }

      // Extract the header informations...
      char id = read_buffer[0];
      char action = read_buffer[1];
      UInt16 target = *((UInt16*)&read_buffer[2]);
      char result = read_buffer[4];


      g_mutex_lock(m_lock);
      if(m_pending != NULL
          && (id == m_pending->Id)
          && (action == m_pending->Action)
          && (target == m_pending->Target))
      {

        m_pending->response.Result = result;
        m_pending->response.Data.Clear();
        for(int i = 5; i < read_buffer.Size(); i++)
        {
          m_pending->response.Data << read_buffer[i];
        }
        m_pending = NULL;
      }//end if
      g_mutex_unlock(m_lock);
    }//end else

  }//end while
}//end mainConnection


bool UdpTransport::sendNotification(Notification& notification)
{
  // Build a notification message and send it on the network.
  write_buffer.Clear();
  write_buffer << (Byte)0xFF;
  notification.to_message(write_buffer);

  return m_client.writeMessage(write_buffer);
}//end sendNotification


bool UdpTransport::subscribe(UInt16 target, Byte mode, Subscription& subscription)
{
    // In this method, we execute an INSERT request on the Notification Manager service,
    // and keep a record of this in m_subscriptions.

    Request r;
    
    r.Action = Request::Insert;
    r.Target = 1; // Notification Manager is always instance 1.
    
    r.Data << target;    
    r.Data << mode;
    
    if(!request(r))
    {
        return false;
    }

    // Add an entry to our subscriptions map.
    // We have to synchronize the access with the RX thread.

    g_mutex_lock(m_lock);
    subscriptionMap[target] = &subscription;
    g_mutex_unlock(m_lock);

    return true;
}//end subscribe

}// Pure