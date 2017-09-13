/**
* Copyright (C) 2017 Chalmers Revere
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
* USA.
*/

#include <iostream>

#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/strings/StringToolbox.h>
#include <opendavinci/odcore/wrapper/Eigen.h>

#include "groundspeed.hpp"

namespace opendlv {
namespace proxy {
namespace cfsd18 {

GroundSpeed::GroundSpeed(int32_t const &a_argc, char **a_argv)
: TimeTriggeredConferenceClientModule(a_argc, a_argv, "proxy-cfsd18-groundspeed")
, m_groundSpeed(100/3.6)
{
}

GroundSpeed::~GroundSpeed()
{
}

void GroundSpeed::nextContainer(odcore::data::Container &/*a_container*/)
{
	/*
	odcore::base::Lock l(m_mutex);

	int32_t dataType = a_container.getDataType();
	if (dataType == opendlv::proxy::GroundSpeedReading::ID()) {
	    auto groundSpeed = a_container.getData<opendlv::proxy::GroundSpeedReading>();*/
	         /* auto propulsion = c.getData<opendlv::proxy::rhino::Propulsion>();
	          const double groundSpeedKph = static_cast<double>(propulsion.getPropulsionShaftVehicleSpeed());
	          const double groundSpeed = groundSpeedKph / 3.6;*/

	          /*opendlv::proxy::GroundSpeedReading groundSpeedReading;
	          groundSpeedReading.setGroundSpeed(groundSpeed);

	          Container groundSpeedReadingContainer = Container(groundSpeedReading);
	          getConference().send(groundSpeedReadingContainer);
  }*/
  
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode GroundSpeed::body()
{
  while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
      odcore::data::dmcp::ModuleStateMessage::RUNNING) {

//      Eigen::AngleAxisd deltaRollAngle(deltaRoll, Eigen::Vector3d::UnitX());

  	double groundSpeed = m_groundSpeed;
  	opendlv::proxy::GroundSpeedReading groundSpeedReading;
	groundSpeedReading.setGroundSpeed(groundSpeed);

	odcore::data::Container groundSpeedReadingContainer(groundSpeedReading);
	getConference().send(groundSpeedReadingContainer);

  }
  return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void GroundSpeed::setUp()
{
  std::string const name = getKeyValueConfiguration().getValue<std::string>(
        "proxy-cfsd18-groundspeed.name");

  if (isVerbose()) {
    std::cout << "Name: " << name << std::endl;
  }
}

void GroundSpeed::tearDown()
{
}

}
}
}
