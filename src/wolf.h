/*
 * wolf.h
 *
 *  Created on: 28/05/2014
 *      \author: jsola
 */

/**
 * \file wolf.h
 * \brief General typedefs for the Wolf project
 * \author Joan Sola
 */
#ifndef WOLF_H_
#define WOLF_H_

//includes from std lib
#include <memory>
#include <list>
#include <map>

//includes from Eigen lib
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <eigen3/Eigen/Sparse>

/**
 * \brief scalar type for the Wolf project
 *
 * This typedef makes it possible to recompile the whole Wolf project with double, float, or other precisions.
 *
 * To change the project precision, just edit wolf.h and change the precision of this typedef.
 *
 * Do NEVER forget to use Wolf scalar definitions when you code!!!
 *
 * Do NEVER use plain Eigen scalar types!!! (This is redundant with the above. But just to make sure you behave!)
 *
 * The ONLY exception to this rule is when you need special precision. The ONLY example by now is the time stamp which uses double.
 */
//typedef float WolfScalar;         // Use this for float, 32 bit precision
typedef double WolfScalar;        // Use this for double, 64 bit precision
//typedef long double WolfScalar;   // Use this for long double, 128 bit precision

namespace WolfConstants{
// use it in odometry covariances for instance.
const double MIN_VARIANCE = 1e-6;

// Wolf standard tolerance
const double EPS = 1e-8;
// Wolf smmmmall tolerance
const double EPS_SMALL = 1e-16;
}

///////////////////////////////////////////
// Construct types for any scalar defined in the typedef scalar_t above
////////////////////////////////////////////
/** \brief Namespace extending Eigen definitions
 *
 * We redefine all algebra and gemoetry types to hold double or single precision floats.
 * The appended letter indicating this is 's', so that we have, e.g.,
 * - VectorXf   Vector of floats - defined by Eigen
 * - VectorXd   Vector of doubles - defined by Eigen
 * - VectorXs   Vector of either double of float, depending on the type \b scalar_t, defined by Wolf.
 * 
 */
namespace Eigen  // Eigen namespace extension
{
// 1. Vectors and Matrices
typedef Matrix<WolfScalar, 2, 2, Eigen::RowMajor> Matrix2s;                ///< 2x2 matrix of real scalar_t type
typedef Matrix<WolfScalar, 3, 3, Eigen::RowMajor> Matrix3s;                ///< 3x3 matrix of real scalar_t type
typedef Matrix<WolfScalar, 4, 4, Eigen::RowMajor> Matrix4s;                ///< 4x4 matrix of real scalar_t type
typedef Matrix<WolfScalar, 7, 7, Eigen::RowMajor> Matrix7s;                ///< 7x7 matrix of real scalar_t type
typedef Matrix<WolfScalar, Dynamic, Dynamic, Eigen::RowMajor> MatrixXs;    ///< variable size matrix of real scalar_t type
typedef Matrix<WolfScalar, 1, 1> Vector1s;                ///< 1-vector of real scalar_t type
typedef Matrix<WolfScalar, 2, 1> Vector2s;                ///< 2-vector of real scalar_t type
typedef Matrix<WolfScalar, 3, 1> Vector3s;                ///< 3-vector of real scalar_t type
typedef Matrix<WolfScalar, 4, 1> Vector4s;                ///< 4-vector of real scalar_t type
typedef Matrix<WolfScalar, 6, 1> Vector6s;                ///< 6-vector of real scalar_t type
typedef Matrix<WolfScalar, 7, 1> Vector7s;                ///< 7-vector of real scalar_t type
typedef Matrix<WolfScalar, Dynamic, 1> VectorXs;          ///< variable size vector of real scalar_t type
typedef Matrix<WolfScalar, 1, 2> RowVector2s;             ///< 2-row-vector of real scalar_t type
typedef Matrix<WolfScalar, 1, 3> RowVector3s;             ///< 3-row-vector of real scalar_t type
typedef Matrix<WolfScalar, 1, 4> RowVector4s;             ///< 4-row-vector of real scalar_t type
typedef Matrix<WolfScalar, 1, 7> RowVector7s;             ///< 7-row-vector of real scalar_t type
typedef Matrix<WolfScalar, 1, Dynamic> RowVectorXs;       ///< variable size row-vector of real scalar_t type

// 2. Quaternions and other rotation things
typedef Quaternion<WolfScalar> Quaternions;               ///< Quaternion of real scalar_t type
typedef AngleAxis<WolfScalar> AngleAxiss;                 ///< Angle-Axis of real scalar_t type
}

/** \brief Enumeration of node locations at Wolf Tree
 *
 * Enumeration of node locations at Wolf Tree.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 * 
 */
typedef enum
{
    TOP, ///< root node location. This is the one that commands jobs down the tree.
    MID, ///< middle nodes. These delegate jobs to lower nodes.
    BOTTOM ///< lowest level nodes. These are the ones that do not delegate any longer and have to do the job.
} NodeLocation;

/** \brief Enumeration of all possible frames
 *
 * Enumeration of all possible frames.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 * 
 */
typedef enum
{
    KEY_FRAME,    ///< marks a key frame. It will stay in the frames window and play at optimizations.
    REGULAR_FRAME ///< marks a regular frame. It does play at optimizations but it will be discarded from the window once a newer frame arrives.
} FrameType;

/** \brief Enumeration of all possible frames
 *
 * Enumeration of all possible frames.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    PO_2D,  ///< marks a 2D frame containing position (x,y) and orientation angle.
    PO_3D   ///< marks a 3D frame containing position (x,y,z) and orientation quaternion (x,y,z,w).
} FrameStructure;

/** \brief Enumeration of all possible constraints
 *
 * Enumeration of all possible constraints.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 * 
 */
typedef enum
{
    CTR_GPS_FIX_2D,				///< marks a 2D GPS Fix constraint.
    CTR_GPS_PR_2D,				///< marks a 2D GPS Pseudorange constraint.
    CTR_GPS_PR_3D,				///< marks a 3D GPS Pseudorange constraint.
    CTR_FIX,                    ///< marks a Fix constraint (for priors).
    CTR_ODOM_2D,                ///< marks a 2D Odometry constraint .
    CTR_CORNER_2D,		        ///< marks a 2D corner constraint .
    CTR_CONTAINER,              ///< marks a 2D container constraint .
    CTR_IMG_PNT_TO_EP           ///< marks a constraint from a image point to a Euclidean 3D point landmark (EP). See https://hal.archives-ouvertes.fr/hal-00451778/document

} ConstraintType;

/** \brief Enumeration of constraint categories
 *
 * Enumeration of constraint categories.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    CTR_ABSOLUTE,   ///< Constraint established with absolute reference.
    CTR_FRAME,      ///< Constraint established with a frame (odometry).
    CTR_FEATURE,    ///< Constraint established with a feature (correspondence).
    CTR_LANDMARK    ///< Constraint established with a landmark (correpondence).

} ConstraintCategory;

/** \brief Enumeration of constraint status
 *
 * Enumeration of constraint status.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    CTR_ACTIVE,   ///< Constraint established with absolute reference.
    CTR_INACTIVE  ///< Constraint established with a frame (odometry).

} ConstraintStatus;

/** \brief Enumeration of jacobian computation method
 *
 * Enumeration of jacobian computation method.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    AUTO,    ///< Auto differentiation (AutoDiffCostFunctionWrapper or ceres::NumericDiffCostFunction).
    NUMERIC, ///< Numeric differentiation (ceres::NumericDiffCostFunction).
    ANALYTIC ///< Analytic jacobians.

} JacobianMethod;

/** \brief Enumeration of all possible state parametrizations
 *
 * Enumeration of all possible state parametrizations.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *. TODO: Check othe attributes also (
 */
typedef enum
{
    ST_VECTOR,		  ///< A vector. No local parametrization.
    ST_QUATERNION		  ///< A 3D orientation represented by a quaternion, with the real part at the last position (Eigen specification).
} StateType;

/** \brief Enumeration of all possible state status
 *
 * Enumeration of all possible state status.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    ST_ESTIMATED,		///< State in estimation (default)
    ST_FIXED,			  ///< State fixed, estimated enough or fixed infrastructure.
} StateStatus;

/** \brief Enumeration of all possible sensor types
 *
 * Enumeration of all possible sensor types.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    ODOM_2D,	    ///< Odometry measurement from encoders: displacement and rotation.
    TWIST_2D,       ///< Twist measurement form encoders or motion command: lineal and angular velocities.
    IMU,		      ///< Inertial measurement unit with 3 acceleros, 3 gyros
    CAMERA,		    ///< Regular pinhole camera
    GPS_FIX,	    ///< GPS fix calculated from a GPS receiver
    GPS_RAW,      ///< GPS pseudo ranges, doppler and satellite ephemerides
    LIDAR,		    ///< Laser Range Finder, 2D
    RADAR,		    ///< Radar
    ABSOLUTE_POSE ///< Full absolute pose (XYZ+quaternion)
} SensorType;

/** \brief Enumeration of all possible landmark types
 *
 * Enumeration of all possible landmark types.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
typedef enum
{
    LANDMARK_POINT,		  ///< A Euclidean point landmark, either 3D or 2D
    LANDMARK_CORNER,	  ///< A corner landmark (2D)
    LANDMARK_CONTAINER	///< A container landmark (2D)
} LandmarkType;

typedef enum
{
    LANDMARK_CANDIDATE,   ///< A landmark, just created. Association with it allowed, but not yet establish an actual constraint for the solver
    LANDMARK_ESTIMATED, ///< A landmark being estimated. Association with it allowed, establishing actual constraints for the solver where both vehicle and landmark states are being estimated
    LANDMARK_FIXED, ///< A landmark estimated. Association with it allowed, establishing actual constraints for the solver, but its value remains static, no longer optimized
    LANDMARK_OUT_OF_VIEW, ///< A landmark out of the field of view. Association with it is not allowed, so does not pose constraints for the solver
    LANDMARK_OLD          ///< An old landmark. Association with it not allowed, but old constraints can still be taken into account by the solver.
} LandmarkStatus;

/** \brief Pending status of a node
 *
 * Enumeration of all possible pending status of a node.
 *
 * You may add items to this list as needed. Be concise with names, and document your entries.
 *
 */
/*typedef enum
{
    NOT_PENDING,	  ///< A point landmark, either 3D or 2D
    ADD_PENDING,	  ///< A corner landmark (2D)
    UPDATE_PENDING	///< A container landmark
} PendingStatus;*/

/////////////////////////////////////////////////////////////////////////
//      TYPEDEFS FOR POINTERS AND ITERATORS IN THE WOLF TREE
/////////////////////////////////////////////////////////////////////////
// - forwards for pointers
//class VehicleBase;

class NodeTerminus;
class WolfProblem;
class MapBase;
class LandmarkBase;
class LandmarkCorner2D;
class TrajectoryBase;
class FrameBase;
class CaptureBase;
class CaptureMotion;
class CaptureLaser2D;
class FeatureBase;
class FeatureCorner2D;
class ConstraintBase;
class RawBase;
class RawLaser2D;
class SensorBase;
class SensorLaser2D;
class TransSensor;
class ProcessorBase;
class StateBlock;
template<unsigned int SIZE> class StatePoint;
class PinHole;

// - Vehicle
// typedef std::shared_ptr<VehicleBase> VehicleShPtr;
// typedef VehicleBase* VehiclePtr;
// typedef std::list<VehicleShPtr> VehicleList;
// typedef VehicleList::iterator VehicleIter;

// TODO: No seria millor que cada classe es defineixi aquests typedefs?

//Problem
//typedef std::shared_ptr<WolfProblem> WolfProblemShPtr;
typedef WolfProblem* WolfProblemPtr;

//Map
typedef std::list<MapBase*> MapBaseList;
typedef MapBaseList::iterator MapBaseIter;

//Landmark
typedef std::list<LandmarkBase*> LandmarkBaseList;
typedef LandmarkBaseList::iterator LandmarkBaseIter;

//Landmark corner 2D
typedef std::list<LandmarkCorner2D*> LandmarkCorner2DList;
typedef LandmarkCorner2DList::iterator LandmarkCorner2DIter;

//Trajectory
// typedef std::list<TrajectoryBaseShPtr> TrajectoryBaseList;
// typedef TrajectoryBaseList::iterator TrajectoryBaseIter;

// - Frame
typedef std::list<FrameBase*> FrameBaseList;
typedef FrameBaseList::iterator FrameBaseIter;

// - Capture
typedef std::list<CaptureBase*> CaptureBaseList;
typedef CaptureBaseList::iterator CaptureBaseIter;

// - Capture Relative
typedef std::list<CaptureMotion*> CaptureRelativeList;
typedef CaptureRelativeList::iterator CaptureRelativeIter;

// - Feature
typedef std::list<FeatureBase*> FeatureBaseList;
typedef FeatureBaseList::iterator FeatureBaseIter;

// - Feature Corner 2D
typedef std::list<FeatureCorner2D*> FeatureCorner2DList;
typedef FeatureCorner2DList::iterator FeatureCorner2DIter;

// - Constraint
typedef std::list<ConstraintBase*> ConstraintBaseList;
typedef ConstraintBaseList::iterator ConstraintBaseIter;

// - Raw

// - Sensors
typedef std::list<SensorBase*> SensorBaseList;
typedef SensorBaseList::iterator SensorBaseIter;

// - transSensor
typedef std::map<unsigned int, TransSensor*> TransSensorMap;
typedef TransSensorMap::iterator TransSensorIter;

// - Processors
typedef std::list<ProcessorBase*> ProcessorBaseList;
typedef ProcessorBaseList::iterator ProcessorBaseIter;


// - State
typedef std::list<StateBlock*> StateBlockList;
typedef StateBlockList::iterator StateBaseIter;

// - Pin hole

///** \brief Enumeration of all possible feature types
// *
// * Enumeration of all possible feature types.
// *
// * You may add items to this list as needed. Be concise with names, and document your entries.
// *
// */
//typedef enum
//{
//    BASE,       ///< A feature of FeatureBase -- just for completeness
//    POINT,      ///< A point feature, either 3D or 2D
//    LINE,       ///< a line feature, 2D or 3D
//    CORNER2D,   ///< a corner feature 2D
//    P_RANGE,    ///< A pseudo-range measurement from GPS satellite
//    V_DOPPLER,  ///< Doppler velocity measurement from GPS satellite
//    GPS_FIX,    ///< A GPS fix
//    LIDAR_SCAN, ///< Full 2D laser scan
//    LIDAR_RAY   ///< A single laser ray
//} FeatureType;


inline WolfScalar pi2pi(const WolfScalar& angle)
{
    return (angle > 0 ? fmod(angle + M_PI, 2 * M_PI) - M_PI : fmod(angle - M_PI, 2 * M_PI) + M_PI);
}

#endif /* WOLF_H_ */
