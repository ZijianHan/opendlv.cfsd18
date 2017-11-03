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

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "opendavinci/odcore/wrapper/SharedMemoryFactory.h"
#include "opendavinci/odcore/wrapper/SharedMemory.h"

#include "odvdopendlvdata/GeneratedHeaders_ODVDOpenDLVData.h"

#include "DetectCone.hpp"

namespace opendlv {
namespace logic {
namespace cfsd18 {
namespace perception {

DetectCone::DetectCone(int32_t const &a_argc, char **a_argv)
: DataTriggeredConferenceClientModule(a_argc, a_argv, "logic-cfsd18-perception-detectcone")
{
}

DetectCone::~DetectCone()
{
}

void DetectCone::nextContainer(odcore::data::Container &c)
{
  if (c.getDataType() != odcore::data::image::SharedImage::ID()) {
    return;
  }

  odcore::data::image::SharedImage mySharedImg = c.getData<odcore::data::image::SharedImage>();
  std::string cameraName = mySharedImg.getName();
  //std::cout << "Received image from camera " << cameraName  << "!" << std::endl;

  odcore::data::TimeStamp timeStampOfImage = c.getReceivedTimeStamp();
  
  std::shared_ptr<odcore::wrapper::SharedMemory> sharedMem(
    odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(mySharedImg.getName()));
  
  const uint32_t nrChannels = mySharedImg.getBytesPerPixel();
  const uint32_t imgWidth = mySharedImg.getWidth();
  const uint32_t imgHeight = mySharedImg.getHeight();

  std::cout << "Good 1" << std::endl;

  IplImage* myIplImage;
  myIplImage = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_8U, nrChannels);
  cv::Mat myImage(myIplImage);

  if (!sharedMem->isValid()) {
    return;
  }
  
  std::cout << "Good 2" << std::endl;

  cv::imshow("[proxy-camera-axis]", myImage);
  cv::waitKey(1);

  sharedMem->lock();
  memcpy(myImage.data, sharedMem->getSharedMemory(), imgWidth*imgHeight*nrChannels);
  sharedMem->unlock();
  
  std::cout << "Good 3" << std::endl;

  cvReleaseImage(&myIplImage);

  std::cout << "Good 4" << std::endl;
}

void DetectCone::setUp()
{
}

void DetectCone::tearDown()
{
}

}
}
}
}
