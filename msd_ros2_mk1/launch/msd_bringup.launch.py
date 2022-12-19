import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import LaunchConfigurationEquals
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import LifecycleNode #, Node

def generate_launch_description():
    declare_arg_namespace = DeclareLaunchArgument(
        'namespace', default_value='', description='Set namespace for tf tree.')
    
    lidar_port = LaunchConfiguration(
        'lidar_port', default='/dev/ttyUSB0')
    
    declare_arg_lidar = DeclareLaunchArgument(
        'lidar', default_value='none', description='Set "none", "rplidara1", "rplidars1", or "urm".')

    declare_arg_lidar_frame = DeclareLaunchArgument(
        'lidar_frame', default_value='laser', description='Set lidar frame name.')

    declare_arg_joydev = DeclareLaunchArgument(
        'joydev', default_value='/dev/input/js0', description='Device file for JoyStick Controller.')

    declare_arg_joyconfig = DeclareLaunchArgument(
        'joyconfig', default_value='dualshock4',description='Keyconfig of joystick controllers: \
                     supported: f710, dualshock3, dualshock4')

    mouse_node = LifecycleNode(
        name='raspimouse', package='raspimouse', executable='raspimouse', output='screen', parameters=[os.path.join(get_package_share_directory('raspimouse_slam'), 'config', 'mouse.yaml')]
    )
    
    rplidara1_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('rplidara1_ros'), 'launch'), '/rplidar.launch.py']),
        launch_arguments={'serial_port': lidar_port, 'frame_id': LaunchConfiguration('lidar_frame')}.items(),
        condition=LaunchConfigurationEquals('lidar', 'rplidara1')
    )
    
    rplidars1_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('rplidars1_ros'), 'launch'), '/rplidar.launch.py']),
        launch_arguments={'serial_port': lidar_port, 'frame_id': LaunchConfiguration('lidar_frame')}.items(),
        condition=LaunchConfigurationEquals('lidar', 'rplidars1')
    )

    # urm node i will descrive later

    description_params = {'lidar': LaunchConfiguration('lidar'), 'lidar_frame': LaunchConfiguration('lidar_frame'), 'namespace': LaunchConfiguration('namespace')}.items()

    display_robot_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('raspimouse_slam'), 'launch/'), 'description.launch.py']),
        launch_arguments=description_params
    )

    teleop_params = {'joydev': LaunchConfiguration('joydev'), 'joyconfig': LaunchConfiguration('joyconfig')}.items()

    teleop_joy_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('raspimouse_slam'), 'launch/'), 'teleop.launch.py']),
        launch_arguments=teleop_params
    )

    ld = LaunchDescription()
    ld.add_action(declare_arg_namespace)
    ld.add_action(declare_arg_lidar)
    ld.add_action(declare_arg_lidar_frame)
    ld.add_action(declare_arg_joyconfig)
    ld.add_action(declare_arg_joydev)

    ld.add_action(mouse_node)
    ld.add_action(rplidara1_launch)
    ld.add_action(rplidars1_launch)
    ld.add_action(display_robot_launch)
    ld.add_action(teleop_joy_launch)

    return ld