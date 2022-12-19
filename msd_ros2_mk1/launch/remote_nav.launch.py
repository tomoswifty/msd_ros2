import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')
    map_yaml_file = LaunchConfiguration('map')
    params_file = LaunchConfiguration('params_file')
    rviz2_file = LaunchConfiguration('rviz2_file')

    declare_arg_map = DeclareLaunchArgument(
        'map',
        description='The full path to the map yaml file.'
    )

    declare_arg_params_file = DeclareLaunchArgument(
        'params_file',
        default_value=os.path.join(
            get_package_share_directory('msd_ros2_mk1'),
            'params',
            'raspimouse.yaml'),
        description='The full path to the param file.'
    )

    declare_arg_rviz2_config_path = DeclareLaunchArgument(
        'rviz2_file', default_value=os.path.join(
            get_package_share_directory('msd_ros2_mk1'),
            'rviz',
            'nav2_view.rviz'),
        description='The full path to the rviz file.'
    )

    nav2_launch_file_dir = os.path.join(
        get_package_share_directory('nav2_bringup'),
        'launch'
    )

    nav2_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            nav2_launch_file_dir, '/bringup_launch.py']),
        launch_arguments={
            'map': map_yaml_file,
            'params_file': params_file,
            'use_sim_time': use_sim_time}.items(),
    )

    rviz2_node = Node(
        name='rviz2', package='rviz2', executable='rviz2', output='screen',arguments=['-d', rviz2_file], parameters=[{'use_sim_time': use_sim_time}]
    )

    ld = LaunchDescription()
    ld.add_action(declare_arg_map)
    ld.add_action(declare_arg_params_file)
    ld.add_action(declare_arg_rviz2_config_path)

    ld.add_action(nav2_node)
    ld.add_action(rviz2_node)

    return ld