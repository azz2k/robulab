/**

*/

#ifndef _UdpTransport_h_
#define _UdpTransport_h_

#include "Tools/BasicTypes.h"
#include "Tools/ServerTypes.h"
#include "Tools/Buffer.h"
#include "UdpClient.h"

#include <string>
#include <iostream>
#include <glib.h>

#include <list>
#include <map>
#include <queue>

namespace Pure
{

/**
parse a value of a specific type from stream
*/
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

template < typename T >
int write(Byte* data, int& pos, T value)
{
  *((T*)(&data[pos])) = value;
  pos += sizeof(T);
  return pos;
}//end write


class RequestManager
{
public:
  int get_message(Byte* data)
  {
    Request& request = get_request();

    int idx = 0;
    write(data, idx, (Byte)0x00); // no id here
    write(data, idx, (Byte)request.Action);
    write(data, idx, (UInt16)request.Target);

    for(int i = 0; i < (int)request.Data.Size(); i++)
    {
      write(data, idx, request.Data[i]);
    }

    return idx;
  }//end get_message


  void handle_message(Byte* data, int size)
  {
    Response& response = get_request().response;

    response.Result = data[4];
    response.Data.Clear();

    for(int i = 5; i < size; i++)
    {
      response.Data << data[i];
    }

    handle_response();
  }//end handle_message

protected:
  virtual Request& get_request() = 0;
  virtual void handle_response() = 0;
};


class NotificationHandler
{
public:
  Notification notification;

  void handle_message(Byte* data, int size)
  {
    UInt16 target = *((UInt16*)&data[1]);
    notification.Target = target;
    
    notification.Data.Clear();
    for(int i = 1; i < size; i++)
    {
      notification.Data << data[i];
    }

    handle_notification(notification);
  }//end handle_message

protected:
  virtual void handle_notification(Notification& notification) = 0;
};


class NotificationSender
{
public:  
  int get_message(Byte* data)
  {
    Notification& notification = get_notification();

    int idx = 0;
    write(data, idx, (Byte)0xFF);
    write(data, idx, notification.Target);

    for(int i = 0; i < notification.Data.Size(); i++)
    {
      write(data, idx, notification.Data[i]);
    }

    return idx;
  }//end get_message

protected:
  virtual Notification& get_notification() = 0;
};


// Holds information relative to a subscription. 
struct Subscription
{
    Subscription()
    {
      m_lock = g_mutex_new();
    }

    ~Subscription()
    {
      g_mutex_free(m_lock);
    }

    void pop()
    {
      g_mutex_lock(m_lock);
      notifications.pop();
      g_mutex_unlock(m_lock);
    }

    Notification front()
    {
      g_mutex_lock(m_lock);
      Notification& n =  notifications.front();
      g_mutex_unlock(m_lock);
      return n;
    }

    bool empty()
    {
      g_mutex_lock(m_lock);
      bool res = notifications.empty();
      g_mutex_unlock(m_lock);
      return res;
    }

    // Incoming notifications are stored in this queue.
    std::queue<Notification> notifications;

    // Used to synchronize access to the queue.
    GMutex* m_lock;
};


// Implements the base functionalities of the PURE protocol, 
// e.g requests, notifications, and subscriptions. 

// When instantiated, UdpTransport will create a thread
// that will handle incoming UDP messages from PURE.
// It is implemented in UdpTransport.RxHandler.

// The public methods give access to the PURE protocol functionalities.
// Typically, a client will first execute a GET request using
// UdpTransport.SendRequest, to retreive the properties of the service.
// It will then use Subscribe to enable notification messages from PURE.

// Once this initialization sequence is complete, a client will typically
// call ReceiveNotification to receive sensor data, performs its processing and 
// send commands using SendNotification.
class UdpTransport
{
public:

    UdpTransport(
        std::string host = "192.168.1.2", 
        UInt16 remotePort = 60000, 
        UInt16 localPort = 60000);

    ~UdpTransport();



  void register_notificationSender(NotificationSender& sender)
  {
    senderList.push_back(&sender);
  }//end register_notificationSender


  void register_notificationHandler(NotificationHandler& handler, UInt16 target)
  {
    handlerMap[target] = &handler;
  }//end register_notificationHandler
     

  void request(RequestManager& requestManager)
  {



    //openRequestList.push_back(&requestManager);
  }//end request

  bool sendNotification(Notification& notification);

  bool subscribe(UInt16 target, Byte mode, Subscription& subscription);

  bool request(Request& request)
  {
    request.Id = ((m_id++) % 253) + 1;

    write_buffer.Clear();
    request.to_message(write_buffer);

    request.response.Data.Clear();

    g_mutex_lock(m_lock);
    m_pending = &request;
    g_mutex_unlock(m_lock);

    m_client.writeMessage(write_buffer);
    bool res = false;

    // wait
    for(int i = 0; i < 10; i++)
    {
      g_usleep(10000);

      g_mutex_lock(m_lock);
      if(m_pending == NULL)
      {
        g_mutex_unlock(m_lock);
        res = true;
        break;
      }
      g_mutex_unlock(m_lock);
    }//end for

    
    g_mutex_lock(m_lock);
    m_pending = NULL;
    g_mutex_unlock(m_lock);
    
    return res;
  }//end request


  void send_receive()
  {
    /*
    // send requests
    while(!openRequestList.empty())
    {
      RequestManager& request = *(openRequestList.front());

      // generate the message
      Int32 size = request.get_message(buffer);
      // generate id
      buffer[0] = ((m_id++) % 253) + 1;
      // send the message
      m_client.writeMessage(buffer, size);
      // queue the manager for response
      pendingRequests[buffer[0]] = &request;

      openRequestList.pop_front();
    }//end while



    // send notifications
    std::list<NotificationSender*>::iterator iter_sender;
    for(iter_sender = senderList.begin(); iter_sender != senderList.end(); ++iter_sender)
    {
      Int32 size = (*iter_sender)->get_message(buffer);
      m_client.writeMessage(buffer, size);
    }//end for
    */
  }//end send_receive

private:
  // open requests
  std::list<RequestManager*> openRequestList;
  // pending requests
  std::map<Byte, RequestManager*> pendingRequests;
  // sender
  std::list<NotificationSender*> senderList;
  // handler
  std::map<UInt16, NotificationHandler*> handlerMap;

  std::map<UInt16, Subscription*> subscriptionMap;
  

  // Incoming notifications are stored in this queue.
  std::queue<Notification> notifications_queue;

  // Used to store a pending request.
  Request* m_pending; 

  // Use to store transmit data.
  Buffer write_buffer;

  // Use to store receive data.
  Buffer read_buffer;

  // Used for Id field in request messages.
  Byte m_id;



  // UDP protocol access.
  UdpClient m_client;

  // Used to synchronize access to the state between
  // the connection thread and the caller thread.
  GMutex* m_lock;

  // 
  GThread* connectionThread;

  // static thread callback
  static void* connection_thread_static(void* ref);

  // main connection loop
  void mainConnection();
};

}// Pure


class StateRequest: public Pure::RequestManager
{
public:
  StateRequest(Pure::UInt16 target)
  {
    request.Action = Pure::Request::Get;
    request.Target = target;
  }
public:
  Pure::Request request;

  virtual Pure::Request& get_request()
  {
    return request;
  }
};//end class StateRequest

class EnableNotifications: public Pure::RequestManager
{
public:
  EnableNotifications(Pure::UInt16 notification_instance, Pure::UInt16 target, Pure::Byte interval)
  {
    request.Action = Pure::Request::Update;
    request.Target = notification_instance;
    request.Data << target;
    request.Data << interval;
  }
protected:
  Pure::Request request;

  virtual Pure::Request& get_request()
  {
    return request;
  }
  virtual void handle_response()
  {
    std::cout << "Notification: " << request.Target << " is " << ((Pure::UInt32)request.response.Result) << std::endl;
  }//end handle_response
};//end class EnableNotifications


class DisableNotifications: public Pure::RequestManager
{
public:
  DisableNotifications(Pure::UInt16 notification_instance, Pure::UInt16 target)
  {
    request.Action = Pure::Request::Delete;
    request.Target = notification_instance;
    request.Data << target;
  }
protected:
  Pure::Request request;

  virtual Pure::Request& get_request()
  {
    return request;
  }
  virtual void handle_response()
  {
    std::cout << "Notification: " << request.Target << " is " << ((Pure::UInt32)request.response.Result) << std::endl;
  }//end handle_response
};//end class DisableNotifications


class EmptyGetRequest : public StateRequest
{
public:
  EmptyGetRequest(Pure::UInt16 target): StateRequest(target)
  {}

  virtual void handle_response()
  {
    std::cout << "Response to " << request.Target << " is " << ((Pure::UInt32)request.response.Result) << std::endl;
  }//end handle_response
};//end class EmptyGetRequest

#endif // _UdpTransport_h_
