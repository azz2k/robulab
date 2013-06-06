#ifdef  WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
 #endif

#include "Robot.h"

int main(int argc, char** argv)
{
  Robot robot;

  for(int i = 0; i < 4; i++)
  {
    robot.move_trans(0.5, 0.2);
    g_usleep(2000000);
    robot.move_trans(-0.5, 0.2);
    robot.move_rot(M_PI/2.0, 0.5);
  }
}
