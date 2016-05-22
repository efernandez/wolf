#ifndef PROCESSOR_BASE_H_
#define PROCESSOR_BASE_H_

// Fwd refs
namespace wolf{
class SensorBase;
class NodeTerminus;
}

//Wolf includes
#include "wolf.h"
#include "node_linked.h"

namespace wolf {

/** \brief base struct for processor parameters
 *
 * Derive from this struct to create structs of processor parameters.
 */
struct ProcessorParamsBase
{
        std::string type;
        std::string name;
};

//class ProcessorBase
class ProcessorBase : public NodeLinked<SensorBase, NodeTerminus>
{
    public:
        ProcessorBase(const std::string& _type, const std::string& _name = "");

        /** \brief Default destructor (not recommended)
         *
         * Default destructor (please use destruct() instead of delete for guaranteeing the wolf tree integrity)
         *
         **/
        virtual ~ProcessorBase();

        unsigned int id();

        virtual void process(CaptureBase* _capture_ptr) = 0;

        /** \brief Vote for KeyFrame generation
         *
         * If a KeyFrame criterion is validated, this function returns true,
         * meaning that it wants to create a KeyFrame at the \b last Capture.
         *
         * WARNING! This function only votes! It does not create KeyFrames!
         */
        virtual bool voteForKeyFrame() = 0;

        virtual bool permittedKeyFrame() final;

        virtual bool keyFrameCallback(FrameBase* _keyframe_ptr, const Scalar& _time_tolerance) = 0;

        SensorBase* getSensorPtr();

    private:
        static unsigned int processor_id_count_;

    protected:
        unsigned int processor_id_;
//        std::string type_id_;
};

}

//#include "problem.h"

namespace wolf {

inline unsigned int ProcessorBase::id()
{
    return processor_id_;
}

inline SensorBase* ProcessorBase::getSensorPtr()
{
    return upperNodePtr();
}

} // namespace wolf

#endif
