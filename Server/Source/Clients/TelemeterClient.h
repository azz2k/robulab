/**

*/

#ifndef _TelemeterClient_h_
#define _TelemeterClient_h_

#include "UdpTransport.h"
#include <vector>

class TelemeterClient
{
public:
  struct Pose2D
  {
      Pure::Float32 x;
      Pure::Float32 y;
      Pure::Float32 rotation;
  };

  struct TelemeterProperties
  {
      Pose2D pose;
      Pure::Float32 fieldOfView;
      Pure::Float32 minDistance;
      Pure::Float32 maxDistance;
  };

public:

  TelemeterClient(Pure::UdpTransport& server, Pure::UInt16 instance)
    : 
    server(server)
  {
    Pure::Request request;
    request.Action = Pure::Request::Get;
    request.Target = instance;

    if(server.request(request))
    {
      parse_properties(request.response);
    }//end if

    server.subscribe(instance, 0x00, subscription);
  }

  ~TelemeterClient()
  {
  }

  void execute()
  {
    while(!subscription.empty())
    {
      Pure::Notification n = subscription.front();
      handle_notification(n);
      subscription.pop();
    }//end while

  }//end execute

  void parse_properties(Pure::Response& response)
  {
    if(response.Result == Pure::Response::OK)
    {

      while(response.Data.Size() > 0)
      {
        TelemeterProperties p;

        response.Data >> p.pose.x;
        response.Data >> p.pose.y;
        response.Data >> p.pose.rotation;

        response.Data >> p.fieldOfView;
        response.Data >> p.minDistance;
        response.Data >> p.maxDistance;

        properties.push_back(p);
      }//end while

    }//end if
  }//end handle_response


  void handle_notification(Pure::Notification& notification)
  {
    Pure::UInt64 timestamp;
    notification.Data >> timestamp;
    measurements.clear();

    while(notification.Data.Size() > 0)
    {
      Pure::Float32 p;
      notification.Data >> p;
      measurements.push_back(p);
    }//end while
  }//end handle_message
  

private:

  Pure::UdpTransport& server;

  Pure::Notification outbound_notification;
  Pure::Notification inbound_notification;

  Pure::Subscription subscription;

public:
  std::vector<TelemeterProperties> properties;
  std::vector<Pure::Float32> measurements;

  int getMinDistanceIdx()
  {
    if(measurements.empty()) return -1;

    int idx = 0;
    Pure::Float32 minDist = measurements[0];

    for(unsigned int i = 1; i < measurements.size(); i++)
    {
      // HACK: ignore measurements closer than 10cm, due to a broken sensor
      if(measurements[i] > 0.1f && measurements[i] < minDist)
      {
        minDist = measurements[i];
        idx = i;
      }
    }
    return idx;
  }//end getMinDistanceIdx
  
  double getMinDistance()
  {
    if(measurements.empty()) return -1.0;

    Pure::Float32 minDist = measurements[0];

    for(unsigned int i = 1; i < measurements.size(); i++)
    {
      // HACK: ignore measurements closer than 10cm, due to a broken sensor
      if(measurements[i] > 0.1f && measurements[i] < minDist)
      {
        minDist = measurements[i];
      }
    }
    return double(minDist);
  }//end getMinDistanceIdx

  double getMinDistanceForward()
  {
    if(measurements.empty()) return -1.0;

    Pure::Float32 minDist = measurements[0];

    for(unsigned int i = 1; i < measurements.size(); i++)
    {
      // HACK: ignore measurements closer than 10cm, due to a broken sensor
      if(measurements[i] > 0.1f && measurements[i] < minDist && fabs(properties[i].pose.rotation) < 1.0)
      {
        minDist = measurements[i];
      }
    }
    return double(minDist);
  }//end getMinDistanceIdx
};

#endif // _TelemeterClient_h_
