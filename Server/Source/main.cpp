
#ifdef  WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
  #include "myconio.h"
#endif //WIN32


#include <string>
#include <iostream>


#include "UdpTransport.h"


#include "Clients/DirectoryClient.h"
#include "Clients/DifferentialDriveClient.h"
#include "Clients/TelemeterClient.h"


class DirectoryRequest : public StateRequest
{
public:
  // Directory Service is by default at 0
  DirectoryRequest() : StateRequest(0)
  {}

  virtual void handle_response()
  {
    if(request.response.Result == Pure::Response::OK)
    {
      Pure::UInt16 instanceNr; 
      Pure::UInt16 serviceType;

      while(request.response.Data.Size() > 0)
      {
        request.response.Data >> serviceType;
        request.response.Data >> instanceNr;
        //std::cout << "[" << instanceNr << "] " << serviceType << std::endl;
        printf("[%d] %#x\n", instanceNr, serviceType);
      }//end while
      std::cout << std::endl;
    }
  }//end handle_response
};//end class DirectoryRequest
  


class DifferentialDriveRequest : public StateRequest
{
public:
  DifferentialDriveRequest(): StateRequest(3)
  {}

  virtual void handle_response()
  {
    if(request.response.Result == Pure::Response::OK)
    {
      Pure::Float32 value;
      while(request.response.Data.Size() > 0)
      {
        request.response.Data >> value;
        std::cout << value << std::endl;
      }//end while
    }//end if
  }//end handle_response
};//end class DirectoryRequest



class SimpleNotificationHandler: public Pure::NotificationHandler
{
public:
  virtual void handle_notification(Pure::Notification& notification)
  {
    for(int i = 0; i < notification.Data.Size(); i++)
    {
      std::cout << notification.Data[i];
    }
    std::cout << std::endl;
  }//end handle_message
};

// TODO: make it config
static const Pure::UInt16 DirectoryID    = 0;
static const Pure::UInt16 NotificationID = 1;
static const Pure::UInt16 IOID           = 2;
static const Pure::UInt16 DifferentialID = 3;
static const Pure::UInt16 LaserID        = 4;
static const Pure::UInt16 LocalizationID = 5;
static const Pure::UInt16 InfraredID     = 6;
static const Pure::UInt16 UltrasoundID   = 7;
static const Pure::UInt16 DriveID        = 8;
static const Pure::UInt16 DiagnosticID   = 9;
static const Pure::UInt16 BatteryID      = 10;
static const Pure::UInt16 TrajectoryID   = 11;


int main(int argc, char** argv)
{
 
  // initialize the communication server
  Pure::UdpTransport server;

  //
  DirectoryClient directoryClient(server);
  
  // register the US sensor
  TelemeterClient usClient(server, UltrasoundID);

  // register the IR sensor
  TelemeterClient irClient(server, InfraredID);

  // register the drive actuator
  DifferentialDriveClient differentialDriveClient(server, DifferentialID);
  

  //SimpleNotificationHandler s;
  //server.register_notificationHandler(s, DifferentialID);

  //DifferentialDriveRequest dr;
  //server.request(dr);


  // run behavior :)
  while(true)
  {
    // motion commands
    Pure::Float32 rot = 0;
    Pure::Float32 speed = 0;


    // read sensory input
    usClient.execute();

    // get the US sensor with the closest measured distance
    int idx = usClient.getMinDistanceIdx();

    // if the distance is closer than 1m
    if(idx != -1 && usClient.measurements[idx] < 1)
    {
      // rotate towards the closest object
      // i.e., towards the sensor which did measure the closest distance
      rot = usClient.properties[idx].pose.rotation;
      
      // if the closest object is in front
      // approch it with the distance of 0.5m
      if(std::fabs(rot) < 0.5)
      {
        speed = usClient.measurements[idx] - 0.5f;
      }
    }//end if
    

    // set the actuators
    differentialDriveClient.setSpeed(true, speed, rot);
    differentialDriveClient.execute();

    // wait for 0.1s = 100ms
    g_usleep(100000);
  }//end while
  
}//end main