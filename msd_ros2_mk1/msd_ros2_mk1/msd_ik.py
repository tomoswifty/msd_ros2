#!user/bin/env python3
# coding: UTF-8

import rclpy
import math
import sys
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException    
from geometry_msgs import Twist

# from time import sleep

# d = 0.600 m 

class IK(Node): # 速度司令をサブスクライブし，逆運動学を計算して，モータ回転数をパブリッシュするクラス

    def __init__(self):
        super().__init__('msd_ik_node')
        self.sub = self.create_subscription(Twist,'cmd_vel',self.cmdvel_cb,10)
        self.timer = self.create_timer(0.01, self.timer_callback)
        self.vel = Twist()
        self.vel.linear.x = 0.0
        self.vel.angular.z = 0.0
        self.V, self.w = 0.0, 0.0
        self.v_r,self.v_l = 0.0, 0.0

    def get_cmdvel(self, msg):
        V = msg.vel.linear.x
        w = msg.vel.angular.z
        return V,w

    def cmdvel_cb(self,msg):
        self.V, self.w = self.get_cmdvel(msg)
        self.get_logger().info(f'V+{self.V: .2f} w={self.w: .2f}[m/s]')

    # def ik(self,V,w):
    #     d = 0.600 # [m]
    #     v_r = V + d * w
    #     v_l = V - d * w
    #     return v_r,v_l

    # def v_to_rpm(self, v_r, v_l):
    #     motor_rpm_r = v_r * 30 / math.pi * 100 / 0.01105 
    #     motor_rpm_l = v_l * 30 / math.pi * 100 / 0.01105 
    #     return motor_rpm_r, motor_rpm_l


def main(args=None):
    rclpy.init(args=args)
    node = IK()

    try:
        node.cmdvel_cb()
        # node.v_to_rpm(v_r, v_l)
    except KeyboardInterrupt:
        print('eyboardInterrupt')
    except ExternalShutdownException:
        sys.exit(1)
    finally:
        rclpy.try_shutdown()