# ~/ros2_ws/src/roover_bringup/launch/bringup.launch.py

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([

        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            arguments=['-d', '/home/livan/ros2_ws/src/roover_bringup/config/roover.rviz']
        )

    ])
