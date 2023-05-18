# msd_ros2
Development directory for msd ros2 version

/msd_ros2_installer はROS2とMSDの自律移動に関する依存パッケージのインストールスクリプトです．

SLAM，Navigation2，micro-ROS for Arduinoなどのパッケージをインストールします．

/msd_ros2_mk1 ROS2 package はROS2パッケージで，ソースコードがあります．

## 実行


jetson = MSD700 で起動しないと動かないノード
```
# micro-ros 起動
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0 -v6


```



PCで起動(できる)ノード
