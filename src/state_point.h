
#ifndef STATE_POINT_H_
#define STATE_POINT_H_

//std includes
#include <iostream>
#include <vector>
#include <cmath>

//Wolf includes
#include "wolf.h"

//class StateBase
template <unsigned int SIZE>
class StatePoint : public StateBase
{
	public:
		static const unsigned int BLOCK_SIZE = SIZE;
        
        /** \brief Constructor with whole state storage and index where starts the state unit
         * 
         * Constructor with whole state storage and index where starts the state unit
         * \param _st_remote is the whole state vector
         * \param _idx is the index of the first element of the state in the whole state vector
         * 
         **/
		StatePoint(Eigen::VectorXs& _st_remote, const unsigned int _idx) :
			StateBase(_st_remote, _idx)
		{
			//
		}

        /** \brief Constructor with scalar pointer
         * 
         * Constructor with scalar pointer
         * \param _st_ptr is the pointer of the first element of the state unit
         * 
         **/
		StatePoint(WolfScalar* _st_ptr) :
			StateBase(_st_ptr)
		{
			//
		}
        
        /** \brief Destructor
         * 
         * Destructor
         * 
         **/
        virtual ~StatePoint()
        {
            //
        }
        
        /** \brief Returns the parametrization of the state unit
		 *
		 * Returns the parametrizationType (see wolf.h) of the state unit
		 *
		 **/
		virtual stateType getStateType() const
		{
			switch (BLOCK_SIZE)
			{
				case 1:
					return ST_POINT_1D;
				case 2:
					return ST_POINT_2D;
				case 3:
					return ST_POINT_3D;
			}
		}

        /** \brief Prints all the elements of the state unit
		 *
		 * Prints all the elements of the state unit
		 *
		 **/
		virtual void printSelf(unsigned int _ntabs = 0, std::ostream& _ost = std::cout) const
		{
			for (uint i = 0; i < BLOCK_SIZE; i++)
			{
				printTabs(_ntabs);
				_ost << *(this->state_ptr_+i) << std::endl;
			}
		}
};
#endif
