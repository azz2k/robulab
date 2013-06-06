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
}
