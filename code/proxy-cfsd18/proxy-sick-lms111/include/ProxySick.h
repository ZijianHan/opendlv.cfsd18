/**
 * proxy-sick - Interface to Sick.
 * Copyright (C) 2016 Chalmers REVERE
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PROXY_PROXYSICK_H
#define PROXY_PROXYSICK_H

#include <stdint.h>

#include <memory>
#include <string>

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/Container.h>
//#include <opendavinci/odcore/wrapper/SerialPort.h>

#include "opendavinci/odcore/wrapper/SharedMemory.h"
//#include <opendavinci/odcore/io/udp/UDPFactory.h>
//#include <opendavinci/odcore/io/udp/UDPReceiver.h>
#include <opendavinci/odcore/io/tcp/TCPConnection.h>

#include "SickStringDecoder.h"

namespace opendlv {
namespace proxy {
namespace cfsd18 {


using namespace std;
using namespace odcore::wrapper;

/**
 * Interface to Sick.
 */
class ProxySick : public odcore::base::module::TimeTriggeredConferenceClientModule {
   private:
    ProxySick(const ProxySick & /*obj*/) = delete;
    ProxySick &operator=(const ProxySick & /*obj*/) = delete;

   public:
    /**
     * Constructor.
     *
     * @param argc Number of command line arguments.
     * @param argv Command line arguments.
     */
    ProxySick(const int &argc, char **argv);

    virtual ~ProxySick();

   private:
    void setUp();
    void tearDown();
    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

   private:
    void status();
    void startScan();
    void stopScan();
    void settingsMode();
    void setCentimeterMode();
    void setBaudrate38400();
    void setBaudrate500k();
//    void openSerialPort(std::string, uint32_t);
   private:
    std::shared_ptr<odcore::io::tcp::TCPConnection> m_sick;
    std::unique_ptr<SickStringDecoder> m_sickStringDecoder;
 //   std::string m_serialPort;
    uint32_t m_baudRate;


    string m_memoryName;   //Name of the shared memory
    uint32_t m_memorySize; //The total size of the shared memory: MAX_POINT_SIZE * NUMBER_OF_COMPONENTS_PER_POINT * sizeof(float), where MAX_POINT_SIZE is the maximum number of points per frame (This upper bound should be set as low as possible, as it affects the shared memory size and thus the frame updating speed), NUMBER_OF_COMPONENTS_PER_POIN=3 (azimuth, distance, intensity) Recommended values: MAX_POINT_SIZE=30000

 //   string m_udpReceiverIP; //"0.0.0.0" to listen to all network interfaces
 //   uint32_t m_udpPort;     //2368 for velodyne

    std::shared_ptr< SharedMemory > m_sickSharedMemory;
 //   std::shared_ptr< odcore::io::udp::UDPReceiver > m_udpreceiver;
};
}
}
}
 // opendlv::core::system::proxy

#endif /*PROXY_PROXYSICK_H*/
