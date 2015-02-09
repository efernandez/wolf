
#ifndef CAPTURE_BASE_H_
#define CAPTURE_BASE_H_

// Forward declarations for node templates
class FrameBase;
class FeatureBase;

//std includes
//

//Wolf includes
#include "wolf.h"
#include "time_stamp.h"
#include "node_linked.h"
#include "frame_base.h"
#include "feature_base.h"
#include "sensor_base.h"

//class CaptureBase
class CaptureBase : public NodeLinked<FrameBase,FeatureBase>
{
    protected:
        TimeStamp time_stamp_; ///< Time stamp
        SensorBasePtr sensor_ptr_; ///< Pointer to sensor
        Eigen::VectorXs data_; ///< raw data
        Eigen::MatrixXs data_covariance_; ///< Noise of the capture
        Eigen::Vector3s sensor_pose_global_; ///< Sensor pose in world frame: composition of the frame pose and the sensor pose. TODO: use state units
        Eigen::Vector3s inverse_sensor_pose_; ///< World pose in the sensor frame: inverse of the global_pose_. TODO: use state units
        
    public:
        CaptureBase(const TimeStamp& _ts, const SensorBasePtr& _sensor_ptr);
        
        CaptureBase(const TimeStamp& _ts, const SensorBasePtr& _sensor_ptr, const Eigen::VectorXs& _data);

        CaptureBase(const TimeStamp& _ts, const SensorBasePtr& _sensor_ptr, const Eigen::VectorXs& _data, const Eigen::MatrixXs& _data_covariance);

        virtual ~CaptureBase();
        
        /** \brief Set link to Frame
         * 
         * Set link to Frame
         *
         **/
        void linkToFrame(const FrameBaseShPtr& _frm_ptr);
        
        /** \brief Adds a Feature to the down node list
         * 
         * Adds a Feature to the down node list
         *
         **/                        
        void addFeature(FeatureBaseShPtr & _ft_ptr);        

        /** \brief Gets up_node_ptr_
         * 
         * Gets up_node_ptr_, which is a pointer to the Frame owning of this Capture
         *
         **/                
        const FrameBasePtr getFramePtr() const;
        
        /** \brief Gets a reference to feature list
         * 
         * Gets a reference to feature list
         *
         **/                        
//         FeatureBaseList & getFeatureList() const;
        
        FeatureBaseList* getFeatureListPtr();
        
        /** \brief Gets a const reference to correspondence list
         *
         * Gets a const reference to correspondence list
         *
         **/
        const CorrespondenceBaseList getCorrespondenceList() const;

        TimeStamp getTimeStamp() const;

        SensorBasePtr getSensorPtr() const;
        
        SensorType getSensorType() const;
        
        void setTimeStamp(const TimeStamp & _ts);
        
        void setTimeStampToNow();

        void setData(unsigned int _size, const WolfScalar *_data);
        
        void setData(const Eigen::VectorXs& _data);

        void setDataCovariance(const Eigen::MatrixXs& _data_cov);
        
        virtual void processCapture();// = 0;

        virtual Eigen::VectorXs computePrior() const = 0;

        /** \brief Generic interface to find correspondences
         *
         * Generic interface to find correspondences between its features and a map (static/slam) or a previous feature
         *
         **/
        virtual void findCorrespondences() = 0;

        virtual void printSelf(unsigned int _ntabs = 0, std::ostream & _ost = std::cout) const;
};
#endif
