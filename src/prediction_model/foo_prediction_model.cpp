#include "amcl_3d/prediction_model/foo_prediction_model.hpp"
#include <iostream>
namespace amcl_3d
{
XorShift128 FooPredictionModel::rand_;
FooPredictionModel::FooPredictionModel()
{
    last_prediction_time_ = ros::Time::now();
    vel_ << 0.0, 0.0, 0.0;
    omega_ << 0.0, 0.0, 0.0;
}
FooPredictionModel::FooPredictionModel(const Eigen::Vector3d &vel,
                                       const Eigen::Vector3d &omega)
    : vel_(vel), omega_(omega)
{
    last_prediction_time_ = ros::Time::now();
}

bool FooPredictionModel::predict(State &state)
{
    const ros::Time current_time = ros::Time::now();
    const double dt_sec =
        (current_time - last_prediction_time_).toSec();
    if (dt_sec > 0.0)
    {
        predict(state, dt_sec);
        last_prediction_time_ = current_time;
    }
    return true;
}

bool FooPredictionModel::predict(State &state, const double dt_sec)
{
    std::normal_distribution<double> noise(0.0, 0.00);

    printf("\n preditc()\n");
    printf("before update\n");
    std::cout << "state.quat[x,y,z,w] = " << state.quat.x() << ", " << state.quat.y() << ", " << state.quat.z() << ", " << state.quat.w() << ", " << std::endl;
    auto euler = state.quat.toRotationMatrix().eulerAngles(0, 1, 2);
    std::cout << "Euler from quaternion in roll, pitch, yaw = "<< std::endl << euler << std::endl;
    std::cout << "state.position = " << state.position.transpose() << std::endl;

    /* linear */
    const Eigen::Matrix3d rot_w2b = state.quat.normalized().toRotationMatrix();
    const Eigen::Vector3d vel_w = rot_w2b.inverse() * vel_;
    // state.position.x() += (vel_w.x() + noise(rand_)) * dt_sec;
    // state.position.y() += (vel_w.y() + noise(rand_)) * dt_sec;
    // state.position.z() += (vel_w.z() + noise(rand_)) * dt_sec;
    state.position.x() += (vel_w.x() ) * dt_sec;
    state.position.y() += (vel_w.y() ) * dt_sec;
    state.position.z() += (vel_w.z() ) * dt_sec;


    std::cout << "rot_w2b = \n" << rot_w2b << std::endl;
    std::cout << "vel_ = " << vel_.transpose() << std::endl;
    std::cout << "vel_w = " << vel_w.transpose() << std::endl;
    std::cout << "omega_ = " << omega_.transpose() << std::endl;


    /* rotation */
    Eigen::Matrix3d skew_omega;
    skew_omega << 0.0, -omega_.z(), omega_.y(),
                  omega_.z(), 0.0, -omega_.x(),
                 -omega_.y(), omega_.x(), 0.0;
    Eigen::Matrix3d rot_w2b_next = rot_w2b + rot_w2b * skew_omega * dt_sec;
    std::cout << "rot_w2b_next = \n" << rot_w2b_next << std::endl;
    rot_w2b_next.col(0) = rot_w2b_next.col(0).normalized();
    rot_w2b_next.col(1) = rot_w2b_next.col(1).normalized();
    rot_w2b_next.col(2) = rot_w2b_next.col(2).normalized();
    state.quat = Quat(rot_w2b_next);


    std::cout << "skew_omega = \n" << skew_omega << std::endl;
    std::cout << "rot_w2b_next = \n" << rot_w2b_next << std::endl;
    printf("after update\n");
    std::cout << "state.quat[x,y,z,w] = " << state.quat.x() << ", " << state.quat.y() << ", " << state.quat.z() << ", " << state.quat.w() << ", " << std::endl;
    auto euler2 = state.quat.toRotationMatrix().eulerAngles(0, 1, 2);
    std::cout << "Euler from quaternion in roll, pitch, yaw = "<< std::endl << euler2 << std::endl;
    std::cout << "state.position = " << state.position.transpose() << std::endl;



    // std::cout << "predict" << std::endl;
    return true;
}

bool FooPredictionModel::measumentLinearVelocity(const Eigen::Vector3d &vel)
{
    vel_ = vel;
    return true;
}
bool FooPredictionModel::measumentAngularVelocity(const Eigen::Vector3d &omega)
{
    omega_ = omega;
    return true;
}

Eigen::Vector3d FooPredictionModel::getLinearVelocity() { return vel_; }
Eigen::Vector3d FooPredictionModel::getAngularVelocity() { return omega_; }

} // namespace amcl_3d
