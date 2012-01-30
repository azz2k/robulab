/**

*/

#ifndef _DifferentialDriveClient_h_
#define _DifferentialDriveClient_h_

#include "UdpTransport.h"

class DifferentialDriveClient
{
public:
  class MotionRequest
  {
  public:
    MotionRequest()
      : 
      move(false),
      linear_speed(0.0),
      angular_speed(0.0)
    {}

    bool move;
    float linear_speed;
    float angular_speed;
  };

  /// Constains the properties of a drive.
  struct Properties
  {   
    Pure::Float32 MaxLinearSpeed; // Maximum linear speed, in m/s.
    Pure::Float32 MinLinearSpeed; // Minimum linear speed, in m/s.
    Pure::Float32 MaxAngularSpeed; // Maximum angular speed, in r/s.
    Pure::Float32 MinAngularSpeed; // Minimum angular speed, in r/s.
    Pure::Float32 MaxLinearAcceleration; // Maximum linear acceleration, in m/s2.
    Pure::Float32 MaxLinearDeceleration; // Minimum linear acceleration, in m/s2.
    Pure::Float32 MaxAngularAcceleration; // Maximum angular acceleration, in r/s2.
    Pure::Float32 MaxAngularDeceleration; // Minimum angular acceleration, in r/s2.
    Pure::Float32 Width; // Distance between the wheels, in m.
  };

  struct Status
  {   
    Pure::UInt64 timestamp;
    Pure::Byte status; // Status - 0 = Disabled, 1 = Enabled, 2 = Error
    Pure::Float32 TargetLinearSpeed; // Target linear speed, in m/s.
    Pure::Float32 CurrentLinearSpeed; // current linear speed, in m/s.
    Pure::Float32 TargetAngularSpeed; // Target angular speed, in r/s.
    Pure::Float32 CurrentAngularSpeed; // Current angular speed, in r/s.
  };

public:

  DifferentialDriveClient(Pure::UdpTransport& server, Pure::UInt16 instance)
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

    inbound_notification.Target = instance;


    server.subscribe(instance, 0x10, subscription);
  }

  ~DifferentialDriveClient()
  {
  }

  void execute()
  {
    server.sendNotification(get_notification());

    while(!subscription.empty())
    {
      Pure::Notification n = subscription.front();
      handle_notification(n);
      subscription.pop();
    }//end while

    std::cout << status.timestamp << ": ";
    std::cout << "LinearSpeed " << status.CurrentLinearSpeed;
    std::cout << "AngularSpeed " << status.CurrentAngularSpeed;
    std::cout << std::endl;

  }//end execute

  void parse_properties(Pure::Response& response)
  {
    if(response.Result == Pure::Response::OK)
    {
      response.Data >> properties.MaxLinearSpeed;
      response.Data >> properties.MinLinearSpeed;
      response.Data >> properties.MaxAngularSpeed;
      response.Data >> properties.MinAngularSpeed;
      response.Data >> properties.MaxLinearAcceleration;
      response.Data >> properties.MaxLinearDeceleration;
      response.Data >> properties.MaxAngularAcceleration;
      response.Data >> properties.MaxAngularDeceleration;
      response.Data >> properties.Width;
    }//end if
  }//end handle_response


  Pure::Notification& get_notification()
  {
    // enable
    inbound_notification.Data.Clear();
    inbound_notification.Data << (Pure::Byte)motion_request.move;
    inbound_notification.Data << motion_request.linear_speed;
    inbound_notification.Data << motion_request.angular_speed;
    return inbound_notification;
  }//end get_message


  void handle_notification(Pure::Notification& notification)
  {
    notification.Data >> status.timestamp;
    notification.Data >> status.status;
    notification.Data >> status.TargetLinearSpeed;
    notification.Data >> status.CurrentLinearSpeed;
    notification.Data >> status.TargetAngularSpeed;
    notification.Data >> status.CurrentAngularSpeed;
  }//end handle_message
  

  void setSpeed(bool move, float linear_speed, float angular_speed)
  {
    motion_request.move = move;
    motion_request.linear_speed = linear_speed;
    motion_request.angular_speed = angular_speed;
  }


private:

  Pure::UdpTransport& server;

  Pure::Notification outbound_notification;
  Pure::Notification inbound_notification;

  Pure::Subscription subscription;

  //
  MotionRequest motion_request;
  Properties properties;
  Status status;

};

#endif // _DifferentialDriveClient_h_
