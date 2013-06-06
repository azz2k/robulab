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

if __name__ == "__main__":
  robot = Robot()
  robot.move_trans(0.2, 0.2)
  time.sleep(3.0)
  robot.move_trans(-0.2, 0.2)
  time.sleep(3.0)
  robot.move_rot(math.pi/2.0, 0.5)
  time.sleep(3.0)
  robot.move_rot(-math.pi/2.0, 0.5)
