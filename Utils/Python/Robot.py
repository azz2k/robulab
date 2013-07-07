from ctypes import cdll
import ctypes
import math
import time

lib = cdll.LoadLibrary('../../Server/dist/libRobuLABLib.so')

class Robot(object):
  def __init__(self):
    self.obj = lib.Robot_new()
  def move_rot(self, drot0, vrot_abs):
    lib.Robot_move_rot(self.obj, ctypes.c_double(drot0), ctypes.c_double(vrot_abs))
  def move_trans(self, dr0, v_abs):
    lib.Robot_move_trans(self.obj, ctypes.c_double(dr0), ctypes.c_double(v_abs))
  def getCurrentPose(self):
    x = ctypes.c_double()
    y = ctypes.c_double()
    rot = ctypes.c_double()
    lib.Robot_getCurrentPose(self.obj, ctypes.byref(x), ctypes.byref(y), ctypes.byref(rot))
    return [x.value, y.value, rot.value]
  def getMinDistanceUS(self):
    dist = ctypes.c_double()
    lib.Robot_getMinDistanceUS(self.obj, ctypes.byref(dist))
    return dist.value
  def getMinDistanceUSForward(self):
    dist = ctypes.c_double()
    lib.Robot_getMinDistanceUSForward(self.obj, ctypes.byref(dist))
    return dist.value

if __name__ == "__main__":
  robot = Robot()
  for i in range(100):
    dist = robot.getMinDistanceUSForward()
    print dist
    time.sleep(0.2)
#  robot.move_trans(0.2, 0.2)
#  time.sleep(3.0)
#  robot.move_trans(-0.2, 0.2)
#  time.sleep(3.0)
#  robot.move_rot(math.pi/2.0, 0.5)
#  time.sleep(3.0)
#  robot.move_rot(-math.pi/2.0, 0.5)
