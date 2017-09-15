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

#include <opendavinci/odcore/base/Mutex.h>
#include <opendavinci/odcore/base/Lock.h>
#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/strings/StringToolbox.h>
#include <opendavinci/odcore/wrapper/Eigen.h>
#include <opendavinci/generated/odcockpit/SimplePlot.h>
#include <opendavinci/generated/odcockpit/RuntimeConfiguration.h>

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
	
	//odcore::base::Lock l(m_mutex);
/*
	int32_t dataType = a_container.getDataType();
	if (dataType == opendlv::proxy::ActuationRequest::ID()) {
	    auto actuationRequestRead = a_container.getData<opendlv::proxy::ActuationRequest>();  
        const double acceleration = static_cast<double>(actuationRequestRead.getGroundSpeed());
        m_groundSpeed += acceleration*0.1;
    }*/
  
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode GroundSpeed::body()
{
  while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
      odcore::data::dmcp::ModuleStateMessage::RUNNING) {

//      Eigen::AngleAxisd deltaRollAngle(deltaRoll, Eigen::Vector3d::UnitX());
	std::cout << "[GROUNDSPEED]" << " Vehicle Speed: " << m_groundSpeed << std::endl;

  	double groundSpeed = m_groundSpeed;
  	opendlv::proxy::GroundSpeedReading groundSpeedReading;
	groundSpeedReading.setGroundSpeed(groundSpeed);

	odcore::data::Container groundSpeedReadingContainer(groundSpeedReading);
	getConference().send(groundSpeedReadingContainer);

          	odcore::base::Mutex inputMutex;
            {
                // Try to read interactively updated values from RuntimeConfiguration object sent from odcockpit.
                odcore::data::Container c = getKeyValueDataStore().get(odcockpit::RuntimeConfiguration::ID());
                std::cout << "[odcockpit] "  "SenderStamp: " << c.getSenderStamp() << " Identifier: " << getIdentifier() << std::endl;
                // Checking for senderStamp allows the use of multiple RuntimeConfiguration plugins in odcockpit.
               // if (c.getSenderStamp() == 0) {
                    odcockpit::RuntimeConfiguration rc = c.getData<odcockpit::RuntimeConfiguration>();
                    
                    odcore::base::Lock l(inputMutex);

                    if (rc.containsKey_MapOfParameters("groundspeed")) {
                        m_groundSpeed = rc.getValueForKey_MapOfParameters("groundspeed");
                        std::cout << "[odcockpit:2] "  "groundspeed: " << m_groundSpeed  << std::endl;
                 //   }
                }
            }
            {
                // Do some calculation as example.
                odcore::base::Lock l(inputMutex);

                // Send "debug" values to odcockpit for display or plotting.
                odcockpit::SimplePlot sp;
                sp.putTo_MapOfValues("groundspeed", m_groundSpeed);
                sp.putTo_MapOfValues("identifier", getIdentifier());

                odcore::data::Container c(sp);
                getConference().send(c);
            }




  }
  return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void GroundSpeed::setUp()
{
  std::string const groundSpeedConst = getKeyValueConfiguration().getValue<std::string>("proxy-cfsd18-groundspeed.groundSpeedConst");
  //m_groundSpeed = groundSpeedConst;

  if (isVerbose()) {
    std::cout << "GroundSpeedConst: " << groundSpeedConst << std::endl;
  }
}

void GroundSpeed::tearDown()
{
}

}
}
}
