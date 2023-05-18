from setuptools import setup

package_name = 'msd_ros2_mk2'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='sugimoto',
    maintainer_email='tomoki.sugimoto.robot@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'tsubaki_motor_to_serial = msd_ros2_mk2.tsubaki_motor_to_serial:main',
        ],
    },
)
