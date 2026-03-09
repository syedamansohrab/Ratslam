#!/usr/bin/env python
import rospy
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry

# This script translates the car's Raw Rotation (IMU) into Robot Odometry

def callback(data):
    odom = Odometry()
    odom.header.stamp = data.header.stamp
    odom.header.frame_id = "odom"
    odom.child_frame_id = "base_link"

    # 1. CONSTANT FORWARD SPEED (We assume 4 m/s because we don't have wheel encoders)
    odom.twist.twist.linear.x = 4.0
    
    # 2. REAL TURNING SPEED (We take this directly from the dataset's Gyroscope)
    # We multiply by 1.5 just to make the turns slightly more visible on the map
    odom.twist.twist.angular.z = data.angular_velocity.z * 1.5

    pub.publish(odom)

rospy.init_node('imu_to_odom_converter')
# Subscribe to the REAL dataset topic
sub = rospy.Subscriber('/davis/left/imu', Imu, callback)
# Publish to the topic RatSLAM is listening to
pub = rospy.Publisher('/ratslam/odom', Odometry, queue_size=10)

print("Translation Active: Converting IMU Rotation -> RatSLAM Odometry")
rospy.spin()
