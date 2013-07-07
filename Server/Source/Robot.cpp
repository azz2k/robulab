#include "Robot.h"

extern "C" {
  Robot* Robot_new() 
  {
    return new Robot();
  }

  void Robot_move_rot(Robot* robot, double drot0, double vrot_abs)
  { 
    robot->move_rot(drot0, vrot_abs);
  }

  void Robot_move_trans(Robot* robot, double dr0, double v_abs)
  { 
    robot->move_trans(dr0, v_abs);
  }
  
  void Robot_setSpeed(Robot* robot, bool move, double vtrans, double vrot)
  {
    robot->setSpeed(move, vtrans, vrot);
  }
  void Robot_getCurrentPose(Robot* robot,double &x, double &y, double &rot)
  {
    robot->getCurrentPose(x, y, rot);
  }
  void Robot_getMinDistanceUS(Robot* robot, double &dist)
  {
    robot->getMinDistanceUS(dist);
  }
}
