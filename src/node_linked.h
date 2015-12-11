/**
 * This file defines the basic template class for linked nodes.
 *
 * As a definition of a template class of a significant complexity,
 * this file is organized with member definitions within the class declaration.
 * This has some advantages:
 * - Functions can be inlined.
 * - typedefs within the class can be used straightforwardly in definitions.
 * - Circular references are minimized.
 */

#ifndef NODE_LINKED_H_
#define NODE_LINKED_H_

class WolfProblem;

//wolf includes
#include "node_base.h"
#include "wolf.h"

/** \brief Linked node element in the Wolf Tree
 * 
 * \param UpperType the type of node one level up in the Wolf tree.
 * \param LowerType the type of node one level down in the Wolf tree.
 *
 * Inherit from this class to implement a node element to be placed somewhere in the Wolf Tree.
 * A node has five main data members:
 * - An unique ID to identify it over the whole Wolf Tree (inherited from Node)
 * - A label indicating the node nature (inherited from Node)
 * - An enum indicating tree location (see NodeLocation enum at wolf.h)
 * - down_node_list_: A list of shared pointers to derived node objects, specified by the template parameter LowerType.
 * - up_node_: A regular pointer to a derived node object, specified by the template parameter UpperType.
 *
 */
template<class UpperType, class LowerType>
class NodeLinked : public NodeBase
{
    public: 
        typedef UpperType* UpperNodePtr;
        typedef LowerType* LowerNodePtr;
        
    protected:
        typedef std::list<LowerNodePtr> LowerNodeList;
        typedef typename LowerNodeList::iterator LowerNodeIter;

    protected:
        NodeLocation location_; ///< Indicates whether this node is a TOP, MIDDLE or BOOTOM node
        UpperNodePtr up_node_ptr_; //it is not a shared pointer because the ownership of upper node should not be shared by lower nodes
        LowerNodeList down_node_list_; ///< A list of pointers to lower nodes
        bool is_deleting_; ///< This node is being deleted.

    public:

        /** \brief Constructor without specify up node
         *
         * Constructor without specify up node
		 * 
         */
        NodeLinked(const NodeLocation _loc, const std::string& _label);

        /** \brief Default destructor
         *
         * Default destructor
		 * 
         */		
        virtual ~NodeLinked();

        /** \brief Wolf destructor
         *
         * Wolf destructor (please use it instead of delete for guaranteeing the wolf tree integrity)
         *
         */
        virtual void destruct() final;

        /** \brief Checks if the destructor has been called already
         *
         * Checks if the destructor has been called already
         *
         */
        virtual const bool isDeleting() const;

        /** \brief Checks if node is on the top of Wolf tree
         *
         * Check if node is on the top of Wolf tree
		 * 
         */		
        bool isTop() const;

        /** \brief Checks if node is at the bottom of Wolf tree
         *
         * Check if node is at the bottom of Wolf tree
		 * 
         */		
        bool isBottom() const;

        /** \brief Sets link to up node
         *
         * Sets link to up node
         *
         */
        void linkToUpperNode(UpperNodePtr _pptr);

        /** \brief Clears link to up node
         *
         * Sets link to up node
         *
         */
        void unlinkFromUpperNode();

        /** \brief Access the pointer to parent.
         *
         * Access the pointer to parent.
         * Throw if parent nullptr.
         *
         */
        const UpperNodePtr upperNodePtr() const;

        /** \brief Gets a reference to parent.
         *
         * Get a reference to parent.
         * Throw if parent is nullptr.
         *
         */
        const UpperType& upperNode() const;

        /** \brief Adds a down node 
         *
         * Adds a down node 
         *
         */		
        void addDownNode(LowerNodePtr _ptr);
		
        /** \brief Gets a reference to down node list
         *
         * Gets a reference to down node list
         *
         */
        LowerNodeList& downNodeList() const;

        /** \brief Gets a pointer to down node list
         *
         * Gets a pointer to down node list
         *
         */
        LowerNodeList* getDownNodeListPtr();

        /** \brief Removes a down node from list, given an iterator
         *
         * Removes a down node from the list
         * @param _iter an iterator to the particular down node in the list that will be removed
         *
         */
        void removeDownNode(const LowerNodeIter& _iter);

        /** \brief Removes a down node from list, given a pointer
         *
         * Removes a down node from the list
         * @param _ptr a pointer to the particular down node in the list that will be removed
         *
         */
        void removeDownNode(const LowerNodePtr _ptr);

        /** \brief Removes a down node from the list, given a node id
         *
         * Removes a down node from the multimap
         * @param _id node id of the node that will nbe removed
         *
         */
        void removeDownNode(const unsigned int _id);
        
        /** \brief Gets a pointer to the tree top node
         * 
         * Gets a pointer to the tree top node
         * TODO: Review if it could return a pointer to a derived class instead of NodeBase JVN: I tried to do so...
         * 
         **/
        virtual WolfProblem* getTop();

        /** \brief Prints node information
         * 
		 * Prints node information.
         * \param _ntabs number of tabulations to print at the left of the printed information
         * \param _ost output stream
         *
         * Overload this function in derived classes to adapt the printed output to each object's relevant info.
		 * 
         */
        virtual void print(unsigned int _ntabs = 0, std::ostream& _ost = std::cout) const;


    protected:

        /** \brief Prints tabulated information about this node.
         *
         * Prints information about this node. It adds a number of tabs given by _ntabs.
         *\param _ntabs the number of tabs.
         *\param _ost the stream it prints to
		 * 
         */
        virtual void printSelf(unsigned int _ntabs = 0, std::ostream& _ost = std::cout) const;

        /** \brief Prints upper node info
         *
         * Prints upper node info
         * 
         **/
        void printUpper(unsigned int _ntabs = 0, std::ostream& _ost = std::cout) const;

        /** \brief Prints recursively lower node info
         *
         * Prints recursively lower node info
         * 
         **/        
        void printLower(unsigned int _ntabs = 0, std::ostream& _ost = std::cout) const;
};

//////////////////////////////////////////
//          IMPLEMENTATION
//////////////////////////////////////////
template<class UpperType, class LowerType>
NodeLinked<UpperType, LowerType>::NodeLinked(const NodeLocation _loc, const std::string& _label) :
        NodeBase(_label), //
        location_(_loc), //
        up_node_ptr_(nullptr),
		down_node_list_(),
		is_deleting_(false)
{
}

template<class UpperType, class LowerType>
NodeLinked<UpperType, LowerType>::~NodeLinked()
{
	//std::cout << "deleting Nodelinked " << node_id_ << " down_node_list_.size() " << down_node_list_.size() << std::endl;
    is_deleting_ = true;

	while (!down_node_list_.empty())
	{
	    delete down_node_list_.front();
	    down_node_list_.pop_front();
	}
}

template<class UpperType, class LowerType>
void NodeLinked<UpperType, LowerType>::destruct()
{
    //std::cout << "destruct() " << node_id_ << " down_node_list_.size() " << down_node_list_.size() << std::endl;
    if (!is_deleting_)
    {
        if (up_node_ptr_ != nullptr && !up_node_ptr_->isTop())
        {
            //std::cout << "upper node is not WolfProblem " << std::endl;
            up_node_ptr_->removeDownNode((typename UpperType::LowerNodePtr)(this));
        }
        else
        {
            //std::cout << "upper node is WolfProblem or nullptr" << std::endl;
            delete this;
        }
    }
}

template<class UpperType, class LowerType>
inline const bool NodeLinked<UpperType, LowerType>::isDeleting() const
{
    return is_deleting_;
}

template<class UpperType, class LowerType>
inline bool NodeLinked<UpperType, LowerType>::isTop() const
{
    if (location_ == TOP)
        return true;
    else
        return false;
}

template<class UpperType, class LowerType>
inline bool NodeLinked<UpperType, LowerType>::isBottom() const
{
    if (location_ == BOTTOM)
        return true;
    else
        return false;
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::linkToUpperNode(UpperNodePtr _pptr)
{
    if (isTop())
    {
        up_node_ptr_ = nullptr;
    }
    else
    {
        up_node_ptr_ = _pptr;
    }
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::unlinkFromUpperNode()
{
    up_node_ptr_ = nullptr;
}

template<class UpperType, class LowerType>
inline const typename NodeLinked<UpperType, LowerType>::UpperNodePtr NodeLinked<UpperType, LowerType>::upperNodePtr() const
{
    assert(up_node_ptr_ != nullptr);
    return up_node_ptr_;
}

template<class UpperType, class LowerType>
inline const UpperType& NodeLinked<UpperType, LowerType>::upperNode() const
{
    assert(up_node_ptr_ != nullptr);
    return *up_node_ptr_;
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::addDownNode(LowerNodePtr _ptr)
{
	assert(!isBottom() && "Trying to add a down node to a bottom node");
	down_node_list_.push_back(_ptr);
	_ptr->linkToUpperNode( (typename LowerType::UpperNodePtr)(this) );
	//std::cout << "node: " << _ptr->nodeId() << " linked to " <<_ptr->upperNodePtr()->nodeId() << std::endl;
}

template<class UpperType, class LowerType>
inline typename NodeLinked<UpperType, LowerType>::LowerNodeList& NodeLinked<UpperType, LowerType>::downNodeList() const
{
    return down_node_list_;
}

template<class UpperType, class LowerType>
inline typename NodeLinked<UpperType, LowerType>::LowerNodeList* NodeLinked<UpperType, LowerType>::getDownNodeListPtr()
{
    return &down_node_list_;
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::removeDownNode(const unsigned int _id)
{
    for (auto iter = down_node_list_.begin(); iter != down_node_list_.end(); ++iter)
    {
        if ( (*iter)->nodeId() == _id)
        {
            removeDownNode(iter);
            break; //avoid comparison of iter and list.end(), otherwise Valgrind claimed
        }
    }
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::removeDownNode(const LowerNodePtr _ptr)
{
    down_node_list_.remove(_ptr);
    delete _ptr;
}

template<class UpperType, class LowerType>
inline void NodeLinked<UpperType, LowerType>::removeDownNode(const LowerNodeIter& _iter)
{
    //(*_iter)->unlinkFromUpperNode();
    down_node_list_.erase(_iter);
    delete *_iter;
}

template<class UpperType, class LowerType>
WolfProblem* NodeLinked<UpperType, LowerType>::getTop()
{
    if (up_node_ptr_ != nullptr)
	    return up_node_ptr_->getTop();
    return nullptr;
}

template<class UpperType, class LowerType>
void NodeLinked<UpperType, LowerType>::print(unsigned int _ntabs, std::ostream& _ost) const
{
    printSelf(_ntabs, _ost); //one line
    
    if ((location_ != TOP) && (up_node_ptr_ != nullptr))
    {
        printUpper(_ntabs, _ost); 
    }
    if (location_ != BOTTOM)
    {
        printLower(_ntabs, _ost);
    }
}

template<class UpperType, class LowerType>
void NodeLinked<UpperType, LowerType>::printSelf(unsigned int _ntabs, std::ostream& _ost) const
{
    printTabs(_ntabs);
    _ost << nodeLabel() << " " << nodeId() << " : ";
    switch (location_)
    {
        case TOP:
            _ost << "TOP" << std::endl;
            break;
        case MID:
            _ost << "MID" << std::endl;
            break;
        case BOTTOM:
            _ost << "BOT" << std::endl;
            break;
        default:
            _ost << "*" << std::endl;
            break;
    }
}

template<class UpperType, class LowerType>
void NodeLinked<UpperType, LowerType>::printUpper(unsigned int _ntabs, std::ostream& _ost) const
{
    printTabs(_ntabs);
    _ost << "\tUpper Node   --> " << up_node_ptr_->nodeId() << std::endl;
}

template<class UpperType, class LowerType>
void NodeLinked<UpperType, LowerType>::printLower(unsigned int _ntabs, std::ostream& _ost) const
{
    printTabs(_ntabs);
    _ost << "\tLower Nodes  ==> [ ";
    for (auto const & down_node_sh_ptr : down_node_list_)
    {
        _ost << down_node_sh_ptr->nodeId() << " ";
    }
    _ost << "]" << std::endl;
    _ntabs++;
    for (auto const & down_node_ptr : down_node_list_)
    {
        down_node_ptr->print(_ntabs, _ost);
    }
}

#endif /* NODE_LINKED_H_ */
