#!/bin/bash

echo "Starting Dependency Installation for Event-Driven RatSLAM..."

# 1. Navigate to the src folder of the active workspace
cd ../

# 2. Clone OpenRatSLAM (replace URL if you used a specific fork)
echo "Cloning OpenRatSLAM..."
git clone https://github.com/davidmichaeli/OpenRatSLAM.git

# 3. Clone rpg_dvs_ros for event messages
echo "Cloning rpg_dvs_ros..."
git clone https://github.com/uzh-rpg/rpg_dvs_ros.git
git clone https://github.com/catkin/catkin_simple.git

# 4. Install ROS dependencies using rosdep
echo "Installing system dependencies via rosdep..."
cd ../
rosdep update
rosdep install --from-paths src --ignore-src -r -y

# 5. Build the workspace
echo "Building the workspace..."
catkin_make

echo "Installation Complete! Don't forget to run: source devel/setup.bash"
