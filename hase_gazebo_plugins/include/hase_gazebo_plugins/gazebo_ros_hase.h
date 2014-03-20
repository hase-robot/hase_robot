/*
 * Copyright (c) 2013, Yujin Robot.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Yujin Robot nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @author Marcus Liebhardt
 *
 * This work has been inspired by Nate Koenig's Gazebo plugin for the iRobot Create.
 */

#ifndef GAZEBO_ROS_HASE_H
#define GAZEBO_ROS_HASE_H

#include <string>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/common/common.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/math/gzmath.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/sensors/sensors.hh>
#include <ros/ros.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf/transform_broadcaster.h>

namespace gazebo
{

class GazeboRosHase : public ModelPlugin
{
public:
  /// Constructor
  GazeboRosHase();
  /// Destructor
  ~GazeboRosHase();
  /// Called when plugin is loaded
  void Load(physics::ModelPtr parent, sdf::ElementPtr sdf);
  /// Called by the world update start event
  void OnUpdate();

private:
  /*
   * Methods
   */
  /// Callback for incoming velocity commands
  void cmdVelCB(const geometry_msgs::TwistConstPtr &msg);
  /// Callback for incoming velocity commands
  //void motorPowerCB(const hase_msgs::MotorPowerPtr &msg);
  /// Callback for resetting the odometry data
  void resetOdomCB(const std_msgs::EmptyConstPtr &msg);
  /// Spin method for the spinner thread
  void spin();
  //  void OnContact(const std::string &name, const physics::Contact &contact); necessary?

  /*
   *  Parameters
   */
  /// ROS node handles (relative & private)
  ros::NodeHandle nh_, nh_priv_;
  /// node name
  std::string node_name_;
  /// extra thread for triggering ROS callbacks
//  boost::shared_ptr<boost::thread> ros_spinner_thread_; necessary?
  /// flag for shutting down the spinner thread
  bool shutdown_requested_;
  /// pointer to the model
  physics::ModelPtr model_;
  /// pointer to simulated world
  physics::WorldPtr world_;
  /// pointer to the update event connection (triggers the OnUpdate callback when event update event is received)
  event::ConnectionPtr update_connection_;
  /// Simulation time on previous update
  common::Time prev_update_time_;
  /// Flag indicating if the motors are turned on or not
  bool motors_enabled_;
  /// Pointers to Gazebo's joints
  physics::JointPtr joints_[2];
  /// Left wheel's joint name
  std::string left_wheel_joint_name_;
  /// Right wheel's joint name
  std::string right_wheel_joint_name_;
  /// ROS publisher for joint state messages
  ros::Publisher joint_state_pub_;
  /// ROS message for joint sates
  sensor_msgs::JointState joint_state_;
  /// ROS subscriber for velocity commands
  ros::Subscriber cmd_vel_sub_;
  /// Simulation time of the last velocity command (used for time out)
  common::Time last_cmd_vel_time_;
  /// Time out for velocity commands in seconds
  double cmd_vel_timeout_;
  /// Speeds of the wheels
  double wheel_speed_cmd_[2];
  /// Max. torque applied to the wheels
  double torque_;
  /// Separation between the wheels
  double wheel_sep_;
  /// Diameter of the wheels
  double wheel_diam_;
  /// Vector for pose
  double odom_pose_[3];
  /// Vector for velocity
  double odom_vel_[3];
  /// Pointer to pose covariance matrix
  double *pose_cov_[36];
  /// Pointer to twist covariance matrix
  double *twist_cov_[36];
  /// ROS publisher for odometry messages
  ros::Publisher odom_pub_;
  /// ROS message for odometry data
  nav_msgs::Odometry odom_;
  /// Flag for (not) publish tf transform for odom -> robot
  bool publish_tf_;
  /// TF transform publisher for the odom frame
  tf::TransformBroadcaster tf_broadcaster_;
  /// TF transform for the odom frame
  geometry_msgs::TransformStamped odom_tf_;
  /// Maximum distance to floor
  int floot_dist_;
  /// Pointer to IMU sensor model
  sensors::ImuSensorPtr imu_;
  /// Storage for the angular velocity reported by the IMU
  math::Vector3 vel_angular_;
  /// ROS publisher for IMU data
  ros::Publisher imu_pub_;
  /// ROS message for publishing IMU data
  sensor_msgs::Imu imu_msg_;
  /// ROS subscriber for reseting the odometry data
  ros::Subscriber odom_reset_sub_;
};

} // namespace gazebo

#endif /* GAZEBO_ROS_HASE_H */