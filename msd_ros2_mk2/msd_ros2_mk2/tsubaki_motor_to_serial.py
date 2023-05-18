import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist # , TransformStamped
from nav_msgs.msg import Odometry
from tf_transformations import quaternion_from_euler
from tf2_ros import TransformBroadcaster # _.transform_broadcaster
import serial
import time
import math

class MotorDrive(Node):
    def __init__(self):
        super().__init__('motor_drive')
        self.linear_x = 0.0
        self.angular_z = 0.0

        # odometry TF
        self._tf_Odompublisher = TransformBroadcaster(self)
        self.x = 0.0
        self.y = 0.0
        self.theta = 0.0

        timer_period = 0.01
        self.period = timer_period
        self.timer = self.create_timer(timer_period, self.timer_callback)
        
        # serial communicate to esp32_s3
        ## set /dev/ttyUSB0 for lidar RPLiDAR S1, set /dev/ttyUSB1 for esp32_s3
        self.ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=None) 

        # publisher
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
        self.odom_msg = Odometry()

        # subscriver
        self.cmd_vel_sub = self.create_subscription(Twist, '/cmd_vel', self.cmd_vel_callback, 10)
        self.cmd_vel_sub

    def cmd_vel_callback(self, msg):
        self.linear_x = msg.linear.x
        self.angular_z = msg.angular.z
    
    # send for esp
    def serial_to_esp(self):
        # write for esp
        self.ser.write(b'Jetson:')
        self.ser.write(str(self.linear_x).encode())
        self.ser.write(b',')
        self.ser.write(str(self.angular_z).encode())
        self.ser.write(b'\r\n')
        # read from esp32_s3
        # self.left_rpm = self.ser.read(str().encode())
        # self.right_rpm = self.ser.read(str().encode())
    
    def timer_callback(self):
        start_time = time.time()
        # odometry
        # calculate forward kinematics for odometry from esp32_s3 output rpm
        V = self.linear_x
        Wz = self.angular_z

        #update pose from user request
        self.x = self.x + self.linear_x * self.period * math.cos(self.theta)
        self.y = self.y + self.linear_x * self.period * math.sin(self.theta)
        self.theta = self.theta
        # updated_yaw = e[2] + self.cmdvel_angular_z*self.period
        q = quaternion_from_euler(0,0, self.theta)

        self.odom_msg.header.stamp = self.get_clock().now().to_msg()
        self.odom_msg.header.frame_id = "odom"
        self.odom_msg.child_frame_id = "base_link"    # or "base_footprint" or "base_link"
        self.odom_msg.pose.pose.position.x = self.x
        self.odom_msg.pose.pose.position.y = self.y
        self.odom_msg.pose.pose.position.z = 0.0
        self.odom_msg.pose.pose.orientation.x = q[0]
        self.odom_msg.pose.pose.orientation.y = q[1]
        self.odom_msg.pose.pose.orientation.z = q[2]
        self.odom_msg.pose.pose.orientation.w = q[3]
        self.odom_msg.pose.covariance[0] = 0.0001       # covariance = 共分散. よくわからん
        self.odom_msg.pose.covariance[7] = 0.0001
        self.odom_msg.pose.covariance[14] = 0.000001	#1e12
        self.odom_msg.pose.covariance[21] = 0.000001	#1e12
        self.odom_msg.pose.covariance[28] = 0.000001	#1e12
        self.odom_msg.pose.covariance[35] = 0.0001
        self.odom_msg.twist.twist.linear.x = V 
        self.odom_msg.twist.twist.linear.y = 0.0
        self.odom_msg.twist.twist.angular.z = Wz
        self.odom_pub.publish(self.odom_msg)

        self.period = time.time() - start_time
        # self.prev


def main(args=None):
    rclpy.init(args=args)
    motor_drive = MotorDrive()
    rclpy.spin(motor_drive)
    motor_drive.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()