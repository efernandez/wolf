
#ifndef TRAJECTORY_BASE_H_
#define TRAJECTORY_BASE_H_

// Fwd refs
namespace wolf{
class Problem;
class FrameBase;
}

//Wolf includes
#include "wolf.h"
#include "node_linked.h"

//std includes


namespace wolf {

//class TrajectoryBase
class TrajectoryBase : public NodeLinked<Problem,FrameBase>
{
    protected:
        FrameStructure frame_structure_; // Defines the structure of the Frames in the Trajectory.
        // TODO: JVN: No seria millor que aixo ho tingui el problem o el wolf_manager? JS: segurament. Pero fixed_size_ seria una de les opcions de moltes...
        unsigned int fixed_size_; // Limits the number of Frames forming the Trajectory
        
    public:
        TrajectoryBase(FrameStructure _frame_sturcture, std::string _type = "");

        /** \brief Default destructor (not recommended)
         *
         * Default destructor (please use destruct() instead of delete for guaranteeing the wolf tree integrity)
         *
         **/        
        virtual ~TrajectoryBase();
        
        /** \brief Add a frame to the trajectory
         **/
        FrameBase* addFrame(FrameBase* _frame_ptr);

        /** \brief Remove a frame to the trajectory
         **/
        void removeFrame(const FrameBaseIter& _frame_iter);

        /** \brief Returns a pointer to Frame list
         **/
        FrameBaseList* getFrameListPtr();

        /** \brief Returns a pointer to last Frame
         **/
        FrameBase* getLastFramePtr();


        /** \brief Returns a list of all constraints in the trajectory thru reference
         **/
        void getConstraintList(ConstraintBaseList & _ctr_list);
        
        FrameStructure getFrameStructure() const;
};

inline void TrajectoryBase::removeFrame(const FrameBaseIter& _frame_iter)
{
    removeDownNode(_frame_iter);
}

inline FrameBaseList* TrajectoryBase::getFrameListPtr()
{
    return getDownNodeListPtr();
}

inline FrameBase* TrajectoryBase::getLastFramePtr()
{
    return getDownNodeListPtr()->back();
}

inline FrameStructure TrajectoryBase::getFrameStructure() const
{
    return frame_structure_;
}


} // namespace wolf

#endif
