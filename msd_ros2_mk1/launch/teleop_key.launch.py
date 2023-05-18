import launch
import launch_ros.actions

def generate_launch_description():
    return launch.LaunchDescription([
        launch_ros.actions.Node(
            prefix='xterm -e', 
            package='msd_ros2_mk1', 
            executable='teleop_key', 
            output='screen')
            ])
