#ifndef FEATURE_GPS_FIX_H_
#define FEATURE_GPS_FIX_H_

//std includes

//Wolf includes
#include "feature_base.h"
#include "constraint_gps_2D.h"

//class FeatureGPSFix
class FeatureGPSFix : public FeatureBase
{
    public:
        /** \brief Constructor from capture pointer and measure dim
         * 
         * \param _capt_ptr a shared pointer to the Capture up node
         * \param _dim_measurement the dimension of the measurement space
         * 
         */
// 		FeatureGPSFix(const CaptureBasePtr& _capt_ptr, unsigned int _dim_measurement);
        FeatureGPSFix(unsigned int _dim_measurement);

        /** \brief Constructor from capture pointer and measure
         *
         * \param _capt_ptr a shared pointer to the Capture up node
         * \param _measurement the measurement
         *
         */
		//FeatureGPSFix(const CaptureBasePtr& _capt_ptr, const Eigen::VectorXs& _measurement);

        /** \brief Constructor from capture pointer and measure
         *
         * \param _capt_ptr a shared pointer to the Capture up node
         * \param _measurement the measurement
         * \param _meas_covariance the noise of the measurement
         *
         */
// 		FeatureGPSFix(const CaptureBasePtr& _capt_ptr, const Eigen::VectorXs& _measurement, const Eigen::MatrixXs& _meas_covariance);
        FeatureGPSFix(const Eigen::VectorXs& _measurement, const Eigen::MatrixXs& _meas_covariance);

        virtual ~FeatureGPSFix();

        /** \brief Generic interface to find constraints
         * 
         * TBD
         * Generic interface to find constraints between this feature and a map (static/slam) or a previous feature
         *
         **/
        virtual void findConstraints();
        
};
#endif