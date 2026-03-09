cmake_minimum_required(VERSION 3.0.2)
project(event_bridge)

## 1. Find ROS packages we need
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  dvs_msgs
  geometry_msgs
  nav_msgs
  roscpp
  sensor_msgs
  std_msgs
  tf
)

## 2. Find OpenCV (Required for image processing)
find_package(OpenCV REQUIRED)

## 3. Declare the catkin package
catkin_package()

## 4. Include headers from catkin and OpenCV
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

## 5. Build the bridge_node executable
add_executable(bridge_node src/bridge_node.cpp)

## 6. Link the libraries (ROS + OpenCV)
target_link_libraries(bridge_node 
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)
