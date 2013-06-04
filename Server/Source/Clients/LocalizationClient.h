/**

*/

#ifndef _LocalizationClient_h_
#define _LocalizationClient_h_

#include "UdpTransport.h"

class LocalizationClient
{
public:
  struct Pose2D
  {
      Pure::Float64 x;
      Pure::Float64 y;
      Pure::Float64 rotation;
  };

public:

  LocalizationClient(Pure::UdpTransport& server, Pure::UInt16 instance)
    : 
    server(server)
  {
    m_instance = instance;

    Pure::Request request;
    request.Action = Pure::Request::Get;
    request.Target = instance;

    if(server.request(request))
    {
      parse_properties(request.response);
    }//end if

    server.subscribe(instance, 0x00, subscription);
  }

  ~LocalizationClient()
  {
  }

  void execute()
  {
    while(!subscription.empty())
    {
      std::cout << "got a notification" << std::endl;
      Pure::Notification n = subscription.front();
      handle_notification(n);
      subscription.pop();
    }//end while

  }//end execute

  void parse_properties(Pure::Response& response)
  {
    if(response.Result == Pure::Response::OK)
    {
      response.Data >> current_pose.x;
      response.Data >> current_pose.y;
      response.Data >> current_pose.rotation;
    }//end if
  }//end handle_response


  void handle_notification(Pure::Notification& notification)
  {
    Pure::UInt64 timestamp;
    notification.Data >> timestamp;
      
    notification.Data >> current_pose.x;
    notification.Data >> current_pose.y;
    notification.Data >> current_pose.rotation;
  }//end handle_message

private:

  Pose2D current_pose;
  Pure::UInt16 m_instance;
  
  Pure::UdpTransport& server;

  Pure::Notification outbound_notification;

  Pure::Subscription subscription;

public:
  void localize()
  {
    Pure::Request request;
    request.Action = Pure::Request::Get;
    request.Target = m_instance;

    if(server.request(request))
    {
      parse_properties(request.response);
    }//end if
  }

  void getCurrentPose(double &x, double &y, double &rotation)
  {
    x = current_pose.x;
    y = current_pose.y;
    rotation = current_pose.rotation;
  }

};

#endif // _LocalizationClient_h_
