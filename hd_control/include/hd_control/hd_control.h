#ifndef HD_CONTROL_H
#define HD_CONTROL_H

#include <cmath>
#include <climits>

#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/QuaternionStamped.h>
#include <geometry_msgs/Point.h>
#include <std_msgs/UInt8.h>

// #include "hd_msgs/ObstacleDetection.h"
#include "hd_depth/obstacle_detection_class.h"
#include <nav_msgs/OccupancyGrid.h>
#include "hd_control/hd_drone_interface.h"
#include "hd_control/hd_state.h"
#include "hd_control/hd_mission.h"
#include <tf/transform_datatypes.h>

#include <Eigen/Geometry>
#include <Eigen/Dense>

namespace hd_control
{
class DroneControl
{
  public:
    DroneControl(ros::NodeHandle *nh, ros::NodeHandle *nh_priv);
    ~DroneControl();
  private:
    ros::NodeHandle nh_;
    ros::NodeHandle nh_priv_;
    DroneInterfacePtr drone_interface_ptr_;
    MissionPtr mission_ptr_;

    ros::Subscriber velocity_control_x_sub_;
    ros::Subscriber velocity_control_y_sub_;
    ros::Subscriber velocity_control_yaw_sub_;

    ros::Subscriber position_track_enable_sub_;
    ros::Subscriber landing_condition_met_sub_;
    ros::Subscriber relanding_condition_met_sub_;
    ros::Subscriber obstacle_detection_sub_;
    ros::Subscriber repulsive_force_sub_;

    ros::Subscriber attitude_sub_;
    ros::Subscriber gps_sub_;
    ros::Subscriber flight_status_sub_;
    ros::Subscriber local_position_sub_;
    
    double velocity_control_effort_x_;
    double velocity_control_effort_y_;
    double velocity_control_effort_yaw_;

    uint8_t flight_status_ = 255;

    sensor_msgs::NavSatFix current_gps_;
    geometry_msgs::Quaternion current_atti_;
    //double current_heading_;
    geometry_msgs::Point current_local_pos_;

    const double descending_speed_ = -0.5;
    const double ascending_speed_ = 0.5;

    bool obstacle_detected_ = false;
    bool position_track_enabled_ = false;
    bool landing_condition_met_ = false;
    bool relanding_condition_met_ = false;

    int obstacle_threshold_ = 100;
    ObstacleState obstacle_;
    // double obstacle_dist_ = 0;
    
    // int obstacle_dir_ = 0; // 0 for left 1 for right
    // double obstacle_running_average_ = 0;
    // double obstacle_alpha_ = 0;

    HDStates drone_state_;

    bool monitoredTakeoff();
    bool monitoredLanding();

    void obstacleCallback(const nav_msgs::OccupancyGrid::ConstPtr &map);

    void repulsiveForceCallback(const geometry_msgs::PointStamped::ConstPtr &rep);

    void gpsCallback(const sensor_msgs::NavSatFix::ConstPtr& msg);

    void localPositionCallback(const geometry_msgs::PointStamped::ConstPtr& msg)
    {
        current_local_pos_ = msg->point;
    }

    void velocityControlEffortXCallback(const std_msgs::Float64 &velocity_control_effort_x_msg)
    {
        velocity_control_effort_x_ = velocity_control_effort_x_msg.data;
    }

    void velocityControlEffortYCallback(const std_msgs::Float64 &velocity_control_effort_y_msg)
    {
        velocity_control_effort_y_ = velocity_control_effort_y_msg.data;
    }

    void velocityControlEffortYawCallback(const std_msgs::Float64 &velocity_control_effort_yaw_msg)
    {
        velocity_control_effort_yaw_ = velocity_control_effort_yaw_msg.data;
    }

    void positionTrackEnableCallback(const std_msgs::Bool &position_track_enable_msg)
    {
        position_track_enabled_ = position_track_enable_msg.data;
        if (position_track_enabled_ && drone_state_.drone_state==DroneState::STATE_SEARCHING)
        {
            drone_state_.drone_state = DroneState::STATE_ALIGNING;
        }
    }

    void landingConditionMetCallback(const std_msgs::Bool &landing_condition_met_msg)
    {
        landing_condition_met_ = landing_condition_met_msg.data;
        if (landing_condition_met_ && drone_state_.drone_state==DroneState::STATE_ALIGNING)
        {
            drone_state_.drone_state = DroneState::STATE_DESCENDING;
        }
    }

    void relandingConditionMetCallback(const std_msgs::Bool &relanding_condition_met_msg)
    {
        relanding_condition_met_ = relanding_condition_met_msg.data;
    }

    void attitudeCallback(const geometry_msgs::QuaternionStamped::ConstPtr& msg)
    {
        current_atti_ = msg->quaternion;
        //tf::Quaternion current_quat_tf;
        // tf::quaternionMsgToTF(current_atti_ , current_quat_tf);
        // Eigen::Quaterniond quat_eigen(current_quat_tf.w(), current_quat_tf.x(), current_quat_tf.y(), current_quat_tf.z());
        // Eigen::Vector3 euler = quat_eigen.toRotationMatrix().eulerAngles(2, 1, 0);
        // current_heading_ = euler[0];

    }

    void flightStatusCallback(const std_msgs::UInt8::ConstPtr& msg)
    {
        flight_status_ = msg->data;
    }

}; // class DroneControl
} // namespace hd_control

#endif
