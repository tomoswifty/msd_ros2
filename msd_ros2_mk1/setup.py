from setuptools import setup
import os
from glob import glob

package_name = 'msd_ros2_mk1'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name), glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Tomoki Sugimoto',
    maintainer_email='whowillcatchatastraw@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'moter_drive = msd_ros2_mk1.moter_drive:main',
            'msd_ik = msd_ros2_mk1.msd_ik:main',
            'teleop_key = msd_ros2_mk1.teleop_key:main',
            'teleop_joy = msd_ros2_mk1.teleop_joy:main',
            
        ],
    },
)
