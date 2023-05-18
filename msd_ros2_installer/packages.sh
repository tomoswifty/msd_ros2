#!/bin/bash

# ros2 tf
sudo apt install -y ros-foxy-tf-transformations
pip3 install transforms3d


# joystick 
sudo apt install -y joystick
sudo apt install -y ros-foxy-teleop-tools ros-foxy-joy*
sudo apt install -y ros-foxy-teleop-twist-*

    # launch command
    # ros2 launch teleop_twist_joy teleop-launch.py

# ros2 slam
sudo apt install -y ros-foxy-slam-toolbox
    # launch command
    # ros2 launch slam_toolbox online_async_launch.py 
    # ros2 run nav2_map_server map_saver_cli -f ~/map
# ros2 nav
sudo apt install -y ros-foxy-navigation2
sudo apt install -y ros-foxy-nav2-bringup 
    # launch command
    # ros2 launch turtlebot3_navigation2 navigation2.launch.py
