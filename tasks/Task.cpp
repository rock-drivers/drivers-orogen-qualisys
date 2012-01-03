/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <base/logging.h>

using namespace qualisys;

Task::Task(std::string const& name)
    : TaskBase(name)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine)
{
}

Task::~Task()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    if (! TaskBase::configureHook())
        return false;

    driver = new Driver();
    driver->setReadTimeout( base::Time::fromSeconds( 1.0 ) );
    driver->connect( _qtm_host.value() );

    std::vector<std::string> labels;
    driver->loadParameters( labels );

    if( find( labels.begin(), labels.end(), _body_label.value() ) == labels.end() )
    {
	// the label was not found
	LOG_ERROR("The label specified is not available on the QTM server.");
	LOG_INFO("%d label(s) available on server.", labels.size() );
	for( size_t i=0; i<labels.size(); i++ )
	    LOG_INFO("%d %s", i, labels[i].c_str());

	return false;
    }

    return true;
}
bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;

    driver->startStreamData( _body_label.value() );

    return true;
}
void Task::updateHook()
{
    TaskBase::updateHook();

    base::Time ts;
    base::Affine3d trans;

    // read new packet and make sure to clean 
    // everything in the buffers
    do {
	if( driver->getTransform( ts, trans ) )
	{
	    base::samples::RigidBodyState rbs;
	    rbs.time = ts;
	    rbs.setTransform( trans );

	    _body_samples.write( rbs );
	}
    } while( driver->hasPacket() );
}
// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
void Task::stopHook()
{
    TaskBase::stopHook();

    driver->stopStreamData();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();

    delete driver;
}

