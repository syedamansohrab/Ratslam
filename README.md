Direct Event-Driven Topological SLAM (event_bridge)

This repository contains a ROS package (event_bridge) that implements a pure event-driven approach to Simultaneous Localization and Mapping (SLAM) using the OpenRatSLAM framework.

Unlike previous hybrid architectures that rely on standard intensity frames or computationally heavy image reconstruction (e.g., E2VID), this implementation performs Direct Event Integration. It uses raw Time Surfaces (Motion History Images) for topological place recognition and transcodes raw IMU gyroscope data into robot odometry to accurately map trajectory curvature.

Author: Syed Aman Sohrab

Dataset: Tested and validated on the MVSEC Dataset (outdoor_day1.bag).

🚀 Key Features

Pure Event Vision: Bypasses intensity images completely. The RatSLAM LocalView is driven directly by continuous Time Surfaces generated from the DVS event stream.

Neuromorphic Odometry: The fix_turn.py node acts as an IMU-to-Odometry transcoder, converting raw angular velocity ($z$-axis) into standard ROS Odometry to drive the Pose Cell network.

High-Speed & HDR Capable: Inherits the natural benefits of event cameras, remaining robust to motion blur and challenging lighting conditions where standard SLAM fails.

🛠️ Prerequisites & Dependencies

This package was developed and tested on Ubuntu 20.04 with ROS Noetic.

You must have the following third-party dependencies installed in your catkin workspace before compiling:

OpenRatSLAM: The core mapping framework.

rpg_dvs_ros: Required for the dvs_msgs definitions to parse the raw event arrays.

Python 3: Required for the custom IMU transcoder script.

📦 Installation

Clone this repository into the src folder of your active catkin workspace:

cd ~/your_workspace/src
git clone [https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git](https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git)


Make sure the Python script is executable:

chmod +x ~/your_workspace/src/event_bridge/fix_turn.py


Build the workspace and source it:

cd ~/your_workspace
catkin_make
source devel/setup.bash


🏃‍♂️ How to Run the Pipeline

Running the full system requires opening multiple terminal windows. Remember to source your workspace in every new terminal (source devel/setup.bash).

1. Launch the RatSLAM Vision & Mapping Core

This will initialize the Event Bridge and open the RatSLAM visualization windows (Local View, Pose Cells, Experience Map).

roslaunch event_bridge ratslam_mvsec.launch


2. Start the IMU Odometry Transcoder

This script subscribes to the raw IMU data from the bag file, calculates the angular velocity, and feeds the turning dynamics into RatSLAM.

python3 src/event_bridge/fix_turn.py


3. Play the MVSEC Dataset

Feed the recorded data into the system.

rosbag play /path/to/your/outdoor_day1_data.bag


4. Visualize the Raw Camera Feed (Optional)

To verify that the generated topological map correlates with the physical environment, you can watch the raw grayscale feed alongside the mapping process.

rosrun image_view image_view image:=/davis/left/image_raw


📊 Expected Output & Visualization

Once the dataset begins playing, you should observe:

Local View Window: Displaying sparse "white dots" on a black background. These are the decaying Time Surfaces representing structural edges in motion.

Pose Cell Network: The red activity "blob" will shift and rotate continuously, driven by the fix_turn.py odometry script.

Experience Map: As the vehicle turns in the dataset, a blue trajectory line will autonomously generate and curve in sync with the physical road geometry.
