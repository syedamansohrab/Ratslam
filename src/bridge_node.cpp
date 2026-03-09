#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
#include <cv_bridge/cv_bridge.h>
#include <dvs_msgs/EventArray.h>
#include <opencv2/opencv.hpp>
#include <tf/transform_broadcaster.h>

// MVSEC Outdoor Day 1 Resolution (DAVIS346)
const int WIDTH = 346;
const int HEIGHT = 260;

class EventRatSLAMBridge {
private:
    ros::NodeHandle nh;
    ros::Subscriber sub_events;
    ros::Subscriber sub_imu;
    ros::Publisher pub_image;
    ros::Publisher pub_odom;

    cv::Mat time_surface;
    ros::Time last_imu_time;
    double current_yaw;
    double x_pos, y_pos;
    
    // Config: Constant speed assumption (m/s)
    // We assume the car moves forward at 4 m/s (approx city driving)
    const double CONSTANT_SPEED = 4.0; 

public:
    EventRatSLAMBridge() {
        // Subscribers
        sub_events = nh.subscribe("/davis/left/events", 1000, &EventRatSLAMBridge::eventCallback, this);
        sub_imu = nh.subscribe("/davis/left/imu", 1000, &EventRatSLAMBridge::imuCallback, this);

        // Publishers for RatSLAM
        pub_image = nh.advertise<sensor_msgs::Image>("/ratslam/camera/image", 1);
        pub_odom = nh.advertise<nav_msgs::Odometry>("/odom", 1);

        // Initialize Variables
        time_surface = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC1);
        current_yaw = 0.0;
        x_pos = 0.0;
        y_pos = 0.0;
        last_imu_time = ros::Time(0);

        ROS_INFO("Event Bridge Initialized. Waiting for events...");
    }

    void imuCallback(const sensor_msgs::Imu::ConstPtr& msg) {
        if (last_imu_time.toSec() == 0) {
            last_imu_time = msg->header.stamp;
            return;
        }

        double dt = (msg->header.stamp - last_imu_time).toSec();
        last_imu_time = msg->header.stamp;

        // 1. Integrate Gyro Z to get Yaw (Rotation)
        // Note: We might need to invert this depending on sensor mounting.
        // For now, we assume standard Z-up.
        current_yaw += msg->angular_velocity.z * dt;

        // 2. Dead Reckoning Position (Assume constant forward speed)
        // x = x + v * cos(theta) * dt
        // y = y + v * sin(theta) * dt
        x_pos += CONSTANT_SPEED * cos(current_yaw) * dt;
        y_pos += CONSTANT_SPEED * sin(current_yaw) * dt;

        publishOdometry(msg->header.stamp);
    }

    void publishOdometry(ros::Time timestamp) {
        nav_msgs::Odometry odom;
        odom.header.stamp = timestamp;
        odom.header.frame_id = "odom";
        odom.child_frame_id = "base_link";

        // Position
        odom.pose.pose.position.x = x_pos;
        odom.pose.pose.position.y = y_pos;
        odom.pose.pose.position.z = 0.0;

        // Orientation (Yaw to Quaternion)
        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(current_yaw);
        odom.pose.pose.orientation = odom_quat;

        // Velocity (Linear & Angular)
        odom.twist.twist.linear.x = CONSTANT_SPEED;
        odom.twist.twist.angular.z = 0.0; // We use pose for RatSLAM usually, but filling this is good practice.

        pub_odom.publish(odom);
    }

    void eventCallback(const dvs_msgs::EventArray::ConstPtr& msg) {
        // 1. Update Time Surface
        for (const auto& e : msg->events) {
            if (e.x >= 0 && e.x < WIDTH && e.y >= 0 && e.y < HEIGHT) {
                // Set pixel to 255 (White)
                time_surface.at<uint8_t>(e.y, e.x) = 255;
            }
        }

        // 2. Fast Decay (Fade out old events)
        // Subtract 40 from every pixel. 
        // If we process at 30Hz, a pixel fades to black in ~6 frames (0.2s)
        time_surface -= 40; 

        // 3. Publish to RatSLAM
        std_msgs::Header header;
        header.stamp = msg->events.back().ts;
        header.frame_id = "camera_link";
        
        sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(header, "mono8", time_surface).toImageMsg();
        pub_image.publish(img_msg);
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "event_bridge_node");
    EventRatSLAMBridge bridge;
    ros::spin();
    return 0;
}
