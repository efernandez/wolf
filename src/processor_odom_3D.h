/**
 * \file processor_odom_3D.h
 *
 *  Created on: Mar 18, 2016
 *      \author: jsola
 */

#ifndef SRC_PROCESSOR_ODOM_3D_H_
#define SRC_PROCESSOR_ODOM_3D_H_

#include "processor_motion.h"


namespace wolf {

/** \brief Processor for 3d odometry integration.
 *
 * This processor integrates motion data in the form of 3D odometry.
 *
 * As it is programmed, this odometry data is extracted from Captures of the type CaptureOdometry3d,
 * which comes in the form of a 6-vector, containing the following components:
 *   - a 3d position increment in the local frame of the robot (dx, dy, dz)
 *   - a 3d orientation increment in the local frame of the robot (roll, pitch, yaw)
 *
 * The produced integrated deltas are in the form of 7-vectors with the following components:
 *   - a 3d position increment in the local frame of the robot (dx, dy, dz)
 *   - a quaternion orientation increment in the local frame of the robot (qx, qy, qz, qw)
 *
 * The produced integrated states are in the form of 7-vectors with the following components:
 *   - a 3d position increment in the local frame of the robot (dx, dy, dz)
 *   - a quaternion orientation increment in the local frame of the robot (qx, qy, qz, qw)
 *
 * The processor integrates data by ignoring the time increment
 * (as it integrates motion directly, not velocities).
 *
 * All frames are assumed FLU (front, left, up).
 */
class ProcessorOdom3d : public ProcessorMotion
{
    public:
        ProcessorOdom3d(WolfScalar _delta_t = 0);
        virtual ~ProcessorOdom3d();
        virtual void data2delta(const Eigen::VectorXs& _data, const WolfScalar _dt, Eigen::VectorXs& _delta);

    protected:
        virtual void preProcess(){}
        virtual void postProcess(){}

    private:
        void xPlusDelta(const Eigen::VectorXs& _x, const Eigen::VectorXs& _delta, Eigen::VectorXs& _x_plus_delta);
        void deltaPlusDelta(const Eigen::VectorXs& _delta1, const Eigen::VectorXs& _delta2, Eigen::VectorXs& _delta1_plus_delta2);
        virtual void deltaMinusDelta(const Eigen::VectorXs& _delta1, const Eigen::VectorXs& _delta2,
                                     Eigen::VectorXs& _delta2_minus_delta1);
        Eigen::VectorXs deltaZero() const;

    private:
        Eigen::Map<const Eigen::Vector3s> p1_, p2_;
        Eigen::Map<Eigen::Vector3s> p_out_;
        Eigen::Map<const Eigen::Quaternions> q1_, q2_;
        Eigen::Map<Eigen::Quaternions> q_out_;
        void remap(const Eigen::VectorXs& _x1, const Eigen::VectorXs& _x2, Eigen::VectorXs& _x_out);
};


inline ProcessorOdom3d::ProcessorOdom3d(WolfScalar _delta_t) :
        ProcessorMotion(PRC_ODOM_3D, _delta_t, 7, 7, 6),
        p1_(nullptr), //, q1_(nullptr)
        p2_(nullptr), //, q1_(nullptr)
        p_out_(nullptr), //, q1_(nullptr)
        q1_(nullptr), //, q1_(nullptr)
        q2_(nullptr), //, q1_(nullptr)
        q_out_(nullptr) //, q1_(nullptr)
{
}

inline ProcessorOdom3d::~ProcessorOdom3d()
{
}

inline void ProcessorOdom3d::data2delta(const Eigen::VectorXs& _data, const WolfScalar _dt, Eigen::VectorXs& _delta)
{
    _delta.head(3) = _data.head(3);
    new (&q_out_) Eigen::Map<Eigen::Quaternions>(_delta.data() + 3);

    Eigen::v2q(_data.tail(3), q_out_); // Better use q_out_, but it is a Map. Overload v2q() with Maps.
}

inline void ProcessorOdom3d::xPlusDelta(const Eigen::VectorXs& _x, const Eigen::VectorXs& _delta, Eigen::VectorXs& _x_plus_delta)
{
    assert(_x.size() == 7 && "Wrong _x vector size");
    assert(_delta.size() == 7 && "Wrong _delta vector size");
    assert(_x_plus_delta.size() == 7 && "Wrong _x_plus_delta vector size");

    remap(_x, _delta, _x_plus_delta);

    p_out_ = p1_ + q1_ * p2_;
    q_out_ = q1_ * q2_;
}

inline void ProcessorOdom3d::deltaPlusDelta(const Eigen::VectorXs& _delta1, const Eigen::VectorXs& _delta2, Eigen::VectorXs& _delta1_plus_delta2)
{
    assert(_delta1.size() == 7 && "Wrong _delta1 vector size");
    assert(_delta2.size() == 7 && "Wrong _delta2 vector size");
    assert(_delta1_plus_delta2.size() == 7 && "Wrong _delta1_plus_delta2 vector size");

    remap(_delta1, _delta2, _delta1_plus_delta2);
    p_out_ = p1_ + q1_ * p2_;
    q_out_ = q1_ * q2_;
}

inline void ProcessorOdom3d::deltaMinusDelta(const Eigen::VectorXs& _delta1, const Eigen::VectorXs& _delta2,
                                             Eigen::VectorXs& _delta2_minus_delta1)
{
    assert(_delta1.size() == 7 && "Wrong _delta1 vector size");
    assert(_delta2.size() == 7 && "Wrong _delta2 vector size");
    assert(_delta2_minus_delta1.size() == 7 && "Wrong _delta2_minus_delta1 vector size");

    remap(_delta1, _delta2, _delta2_minus_delta1);
    p_out_ = q1_.conjugate() * (p2_ - p1_);
    q_out_ = q1_.conjugate() * q2_;
}

inline Eigen::VectorXs ProcessorOdom3d::deltaZero() const
{
    Eigen::VectorXs delta_zero(7);
    delta_zero << 0, 0, 0, 0, 0, 0, 1;;
    return delta_zero;
}

inline void ProcessorOdom3d::remap(const Eigen::VectorXs& _x1, const Eigen::VectorXs& _x2, Eigen::VectorXs& _x_out)
{
    //            std::cout << "Remap -----------------------------------------" << std::endl;
    //            std::cout << "_x1:    " << _x1.transpose() << std::endl;
    //            std::cout << "_x2:    " << _x2.transpose() << std::endl;
    //            std::cout << "_x_out: " << _x_out.transpose() << std::endl;
    //            _x_out << 1, 2, 3, 4, 5, 6, 7; // put some values to ckech the outputs below
    //            std::cout << "_x_out: " << _x_out.transpose() << std::endl;
    new (&p1_) Eigen::Map<const Eigen::Vector3s>(_x1.data());
    new (&q1_) Eigen::Map<const Eigen::Quaternions>(_x1.data() + 3);
    new (&p2_) Eigen::Map<const Eigen::Vector3s>(_x2.data());
    new (&q2_) Eigen::Map<const Eigen::Quaternions>(_x2.data() + 3);
    new (&p_out_) Eigen::Map<Eigen::Vector3s>(_x_out.data());
    new (&q_out_) Eigen::Map<Eigen::Quaternions>(_x_out.data() + 3);
    //            std::cout << "p1_: " << p1_.transpose() << std::endl;
    //            std::cout << "q1_: " << q1_.coeffs().transpose() << std::endl;
    //            std::cout << "p2_: " << p2_.transpose() << std::endl;
    //            std::cout << "q2_: " << q2_.coeffs().transpose() << std::endl;
    //            std::cout << "p_out_: " << p_out_.transpose() << std::endl;
    //            std::cout << "q_out_: " << q_out_.coeffs().transpose() << std::endl;
    //            std::cout << "-----------------------------------------------" << std::endl;
}

} // namespace wolf

#endif /* SRC_PROCESSOR_ODOM_3D_H_ */
