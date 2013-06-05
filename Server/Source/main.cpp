
#ifdef  WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
//  #include "myconio.h"
#endif //WIN32


#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>

#include "UdpTransport.h"


#include "Clients/DirectoryClient.h"
#include "Clients/DifferentialDriveClient.h"
#include "Clients/TelemeterClient.h"
#include "Clients/LocalizationClient.h"


/*class DirectoryRequest : public StateRequest
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
};*/

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

class Robot
{
  public:
    Robot()
    {
      // initialize the communication server
      server = new Pure::UdpTransport;
      // register the directory client
      directoryClient = new DirectoryClient(*server);
      // register the US sensor
      usClient = new TelemeterClient(*server, UltrasoundID);
      // register the IR sensor
      irClient = new TelemeterClient(*server, InfraredID);
      // register the drive actuator
      differentialDriveClient = new DifferentialDriveClient(*server, DifferentialID);
      // register the localization client
      localizationClient = new LocalizationClient(*server, LocalizationID);
    }

    ~Robot () {} //todo: delete stuff
  private:
    Pure::UdpTransport *server;
    DirectoryClient *directoryClient;
    TelemeterClient *usClient;
    TelemeterClient *irClient;
    DifferentialDriveClient *differentialDriveClient;
    LocalizationClient *localizationClient;
  public:
    void move_rot(double drot0, double vrot_abs)
    {
      double drot = drot0;
      double vrot = vrot_abs*drot/fabs(drot);
      double x0, y0, rot0;
      double x, y, rot;
      
      localizationClient->execute();
      localizationClient->getCurrentPose(x0, y0, rot0);
      x = x0;
      y = y0;
      rot = rot0;
   
      double interval = 50000.0;
      double precision = 1e-3;
      while(fabs(rot-rot0 - drot0) > precision)
      {
        vrot = vrot_abs*drot/fabs(drot);
        if(fabs(drot) < 80.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/2.0;
        if(fabs(drot) < 40.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/4.0;
        if(fabs(drot) < 20.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/8.0;
        if(fabs(drot) < 10.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/16.0;
        if(fabs(drot) < 5.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/32.0;
        if(fabs(drot) < 2.0*precision)
          vrot = vrot_abs*drot/fabs(drot)/64.0;
        std::cout << vrot << std::endl;
        differentialDriveClient->setSpeed(true, 0.0, vrot);
        differentialDriveClient->execute();
        if(drot/vrot > interval / 1000000.0)
          g_usleep(interval);
        else
          g_usleep(drot/vrot * 1000000.0);
        localizationClient->execute();
        localizationClient->getCurrentPose(x, y, rot);
        drot = rot0 + drot0 - rot;
        std::cout << "rot " << drot0 << " " << drot << " " << rot-rot0 << std::endl;
      }
      differentialDriveClient->setSpeed(false, 0.0, 0.0);
      differentialDriveClient->execute();

      localizationClient->execute();
      localizationClient->getCurrentPose(x, y, rot);
      std::cout << "rot " << drot0 << " " << rot-rot0 << std::endl;
    }
};

int main(int argc, char** argv)
{
  Robot robot;
  robot.move_rot(-0.5, 0.5);
  robot.move_rot(0.5, 0.5);
/*  // initialize the communication server
  Pure::UdpTransport server;
  // register the directory client
  DirectoryClient directoryClient(server);
  // register the US sensor
  TelemeterClient usClient(server, UltrasoundID);
  // register the IR sensor
  TelemeterClient irClient(server, InfraredID);
  // register the drive actuator
  DifferentialDriveClient differentialDriveClient(server, DifferentialID);
  // register the localization client
  LocalizationClient localizationClient(server, LocalizationID);
 
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
  }//end while */
  
}//end main
