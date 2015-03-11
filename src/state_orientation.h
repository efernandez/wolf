
#ifndef STATE_ORIENTATION_H_
#define STATE_ORIENTATION_H_

//std includes
#include <iostream>
#include <vector>
#include <cmath>

//Wolf includes
#include "wolf.h"
#include "state_base.h"

//class StateOrientation
class StateOrientation : public StateBase
{
	public:
        
        /** \brief Constructor with whole state storage and index where starts the state unit
         * 
         * Constructor with whole state storage and index where starts the state unit
         * \param _st_remote is the whole state vector
         * \param _idx is the index of the first element of the state in the whole state vector
         * 
         **/
		StateOrientation(Eigen::VectorXs& _st_remote, const unsigned int _idx);

        /** \brief Constructor with scalar pointer
         * 
         * Constructor with scalar pointer
         * \param _st_ptr is the pointer of the first element of the state unit
         * 
         **/
		StateOrientation(WolfScalar* _st_ptr);
        
        /** \brief Destructor
         * 
         * Destructor
         * 
         **/
        virtual ~StateOrientation();

        /** \brief Returns the 3x3 rotation matrix of the orientation
		 *
		 * Returns the 3x3 rotation matrix of the orientation
		 *
		 **/
		virtual Eigen::Matrix3s getRotationMatrix() const = 0;
};
#endif
