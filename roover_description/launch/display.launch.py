from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

from ament_index_python.packages import get_package_share_directory
import os
import xacro

def generate_launch_description():
    # URDF pad
    urdf_path = os.path.join(
        get_package_share_directory('roover_description'),
        'urdf',
        'roover.urdf.xacro'
    )

    # Xacro → URDF XML string
    robot_description_xml = xacro.process_file(urdf_path).toxml()
    robot_description = {'robot_description': robot_description_xml}

    # RViz config pad
    rviz_config_path = PathJoinSubstitution([
        FindPackageShare('roover_bringup'),
        'config',
        'roover.rviz'
    ])

    return LaunchDescription([
        # Robot State Publisher
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            parameters=[robot_description],
            output='screen'
        ),

        # RViz2 openen
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_path],
            output='screen'
        ),
    ])
