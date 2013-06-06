#define _USE_MATH_DEFINES

#ifdef  WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
 #endif

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>

#include "UdpTransport.h"

#include "Clients/DirectoryClient.h"
#include "Clients/DifferentialDriveClient.h"
#include "Clients/TelemeterClient.h"
#include "Clients/LocalizationClient.h"

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
    template <typename T> int sgn(T val)
    {
      return (T(0) < val) - (val < T(0));
    }
    void move_rot(double drot0, double vrot_abs)
    {
      double drot = drot0;
      double vrot = vrot_abs*sgn(drot);
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
        vrot = vrot_abs*sgn(drot);
        if(fabs(drot) < 64.0*precision)
          vrot /= fmin(16.0, pow(2.0, 7-log(fabs(drot))/log(2.0)));
        differentialDriveClient->setSpeed(true, 0.0, vrot);
        differentialDriveClient->execute();
        if(drot/vrot > interval / 1000000.0)
          g_usleep(interval);
        else
          g_usleep(drot/vrot * 1000000.0);
        localizationClient->execute();
        localizationClient->getCurrentPose(x, y, rot);
        drot = rot0 + drot0 - rot;
        std::cout 
          << "drot0 " << drot0 
          << " rot0 " << rot0 
          << " rot " << rot 
          << " rot0-rot " << rot0-rot 
          << " drot " << drot
          << " vrot " << vrot
          << std::endl;
      }
      differentialDriveClient->setSpeed(false, 0.0, 0.0);
      differentialDriveClient->execute();
    }
    
    void move_trans(double dr0, double v_abs)
    {
      double r = 0.0;
      double dr = fabs(dr0) - r;
      double v = v_abs*sgn(dr0);
      double x0, y0, rot0;
      double x, y, rot;
      
      localizationClient->execute();
      localizationClient->getCurrentPose(x0, y0, rot0);
      x = x0;
      y = y0;
      rot = rot0;
   
      double interval = 50000.0;
      double precision = 1e-3;
      while(fabs(r - fabs(dr0)) > precision)
      {
        std::cout << "foo" << std::endl;
        v = v_abs*sgn(dr)*sgn(dr0);
        if(fabs(dr) < 64.0*precision)
          v /= fmin(16.0, pow(2.0, 7-log(fabs(dr))/log(2.0)));
        differentialDriveClient->setSpeed(true, v, 0.0);
        differentialDriveClient->execute();
        if(fabs(dr/v) > interval / 1000000.0)
          g_usleep(interval);
        else
          g_usleep(fabs(dr/v) * 1000000.0);
        localizationClient->execute();
        localizationClient->getCurrentPose(x, y, rot);
        r = sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
        dr = fabs(dr0) - r;
        std::cout 
          << "dr0 " << dr0 
          << " r " << r 
          << " dr " << dr
          << " v " << v
          << std::endl;
      }
      differentialDriveClient->setSpeed(false, 0.0, 0.0);
      differentialDriveClient->execute();
    }
};
