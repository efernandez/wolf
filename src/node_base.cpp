#include "node_base.h"

//init static node counter
unsigned int NodeBase::node_id_count_ = 0;

NodeBase::NodeBase(std::string _label) :
        label_(_label), //
        node_id_(++node_id_count_),
		node_pending_(ADD_PENDING)
{
    //    std::cout << "NodeID::constructor. Id: " << node_id_ << std::endl;
}

NodeBase::~NodeBase()
{
    //    std::cout << "NodeID::destructor. Id: " << node_id_ << std::endl;
}

unsigned int NodeBase::nodeId() const
{
	return node_id_;
}

std::string NodeBase::nodeLabel() const
{
	return label_;
}

PendingStatus NodeBase::getPendingStatus() const
{
	return node_pending_;
}

void NodeBase::setPendingStatus(PendingStatus _pending)
{
	node_pending_ = _pending;
}

void NodeBase::print(unsigned int _ntabs, std::ostream& _ost) const
{
	_ost << label_ << " " << node_id_ << std::endl;
}

void NodeBase::printTabs(unsigned int _ntabs, std::ostream& _ost) const
{
		for (unsigned int i = 0; i < _ntabs; i++) _ost << "\t";
}

