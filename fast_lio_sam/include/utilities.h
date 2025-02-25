#ifndef FAST_LIO_SAM_UTILITY_H
#define FAST_LIO_SAM_UTILITY_H

///// common headers
#include <string>
#include <execution>
///// ROS
#include <ros/ros.h>
#include <tf/LinearMath/Quaternion.h> // to Quaternion_to_euler
#include <tf/LinearMath/Matrix3x3.h> // to Quaternion_to_euler
#include <tf/transform_datatypes.h> // createQuaternionFromRPY
#include <tf_conversions/tf_eigen.h> // tf <-> eigen
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/PointCloud2.h>
///// PCL
#include <pcl/point_types.h> //pt
#include <pcl/point_cloud.h> //cloud
#include <pcl/conversions.h> //ros<->pcl
#include <pcl_conversions/pcl_conversions.h> //ros<->pcl
///// Eigen
#include <Eigen/Eigen> // whole Eigen library: Sparse(Linearalgebra) + Dense(Core+Geometry+LU+Cholesky+SVD+QR+Eigenvalues)
///// GTSAM
#include <gtsam/geometry/Rot3.h>
#include <gtsam/geometry/Point3.h>
#include <gtsam/geometry/Pose3.h>

using namespace std;
//////////////////////////////////////////////////////////////////////
///// conversions
gtsam::Pose3 pose_eig_to_gtsam_pose(const Eigen::Matrix4d &pose_eig_in)
{
  double r_, p_, y_;
  tf::Matrix3x3 mat_;
  tf::matrixEigenToTF(pose_eig_in.block<3, 3>(0, 0), mat_);
  mat_.getRPY(r_, p_, y_);
  return gtsam::Pose3(gtsam::Rot3::RzRyRx(r_, p_, y_), gtsam::Point3(pose_eig_in(0, 3), pose_eig_in(1, 3), pose_eig_in(2, 3)));
}
Eigen::Matrix4d gtsam_pose_to_pose_eig(const gtsam::Pose3 &gtsam_pose_in)
{
	Eigen::Matrix4d pose_eig_out_ = Eigen::Matrix4d::Identity();
	tf::Quaternion quat_ = tf::createQuaternionFromRPY(gtsam_pose_in.rotation().roll(), gtsam_pose_in.rotation().pitch(), gtsam_pose_in.rotation().yaw());
  tf::Matrix3x3 mat_(quat_);
	Eigen::Matrix3d tmp_rot_mat_;
  tf::matrixTFToEigen(mat_, tmp_rot_mat_);
  pose_eig_out_.block<3, 3>(0, 0) = tmp_rot_mat_;
  pose_eig_out_(0, 3) = gtsam_pose_in.translation().x();
  pose_eig_out_(1, 3) = gtsam_pose_in.translation().y();
  pose_eig_out_(2, 3) = gtsam_pose_in.translation().z();
  return pose_eig_out_;
}
geometry_msgs::PoseStamped pose_eig_to_pose_stamped(const Eigen::Matrix4d &pose_eig_in, string frame_id="map")
{
	double r_, p_, y_;
  tf::Matrix3x3 mat_;
  tf::matrixEigenToTF(pose_eig_in.block<3, 3>(0, 0), mat_);
  mat_.getRPY(r_, p_, y_);
  tf::Quaternion quat_ = tf::createQuaternionFromRPY(r_, p_, y_);
	geometry_msgs::PoseStamped pose_;
	pose_.header.frame_id = frame_id;
	pose_.pose.position.x = pose_eig_in(0, 3);
	pose_.pose.position.y = pose_eig_in(1, 3);
	pose_.pose.position.z = pose_eig_in(2, 3);
	pose_.pose.orientation.w = quat_.getW();
	pose_.pose.orientation.x = quat_.getX();
	pose_.pose.orientation.y = quat_.getY();
	pose_.pose.orientation.z = quat_.getZ();
	return pose_;
}
geometry_msgs::PoseStamped gtsam_pose_to_pose_stamped(const gtsam::Pose3 &gtsam_pose_in, string frame_id="map")
{
	tf::Quaternion quat_ = tf::createQuaternionFromRPY(gtsam_pose_in.rotation().roll(), gtsam_pose_in.rotation().pitch(), gtsam_pose_in.rotation().yaw());
	geometry_msgs::PoseStamped pose_;
	pose_.header.frame_id = frame_id;
	pose_.pose.position.x = gtsam_pose_in.translation().x();
	pose_.pose.position.y = gtsam_pose_in.translation().y();
	pose_.pose.position.z = gtsam_pose_in.translation().z();
	pose_.pose.orientation.w = quat_.getW();
	pose_.pose.orientation.x = quat_.getX();
	pose_.pose.orientation.y = quat_.getY();
	pose_.pose.orientation.z = quat_.getZ();
	return pose_;
}
template <typename T>
sensor_msgs::PointCloud2 pcl_to_pcl_ros(pcl::PointCloud<T> cloud, string frame_id="map")
{
  sensor_msgs::PointCloud2 cloud_ROS;
  pcl::toROSMsg(cloud, cloud_ROS);
  cloud_ROS.header.frame_id = frame_id;
  return cloud_ROS;
}

///// transformation
template <typename T>
pcl::PointCloud<T> tf_pcd(const pcl::PointCloud<T> &cloud_in, const Eigen::Matrix4d &pose_tf)
{
	if (cloud_in.size() == 0) return cloud_in;
	pcl::PointCloud<T> pcl_out_ = cloud_in;
	std::for_each(std::execution::par_unseq, pcl_out_.begin(), pcl_out_.end(), [&](T &pt)
	{
		float x_ = pt.x;
		float y_ = pt.y;
		float z_ = pt.z;
		pt.x = pose_tf(0, 0) * x_ + pose_tf(0, 1) * y_ + pose_tf(0, 2) * z_ + pose_tf(0, 3);
		pt.y = pose_tf(1, 0) * x_ + pose_tf(1, 1) * y_ + pose_tf(1, 2) * z_ + pose_tf(1, 3);
		pt.z = pose_tf(2, 0) * x_ + pose_tf(2, 1) * y_ + pose_tf(2, 2) * z_ + pose_tf(2, 3);
	});
  return pcl_out_;
}



#endif