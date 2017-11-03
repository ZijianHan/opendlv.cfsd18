/**
 * Copyright (C) 2015 Chalmers REVERE
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

#include <stdint.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>

#include "SickStringDecoder.h"

namespace opendlv {
namespace proxy {
namespace cfsd18 {


using namespace std;

SickStringDecoder::SickStringDecoder(odcore::io::conference::ContainerConference &a_conference, const double &a_x, const double &a_y, const double &a_z)
    : m_conference(a_conference)
    , m_header(false)
    , m_startConfirmed(false)
    , m_latestReading()
    , m_position()
    , m_measurements()
    , m_startResponse()
    , m_measurementHeader()
    , m_minimumMessageLength(10)
    , m_buffer()
    , m_lidarDataFile()
    , m_messageFile()
    , m_headerFile()
    

    
{
  m_position[0] = a_x;
  m_position[1] = a_y;
  m_position[2] = a_z;

  m_startResponse[0] = 0x06;
  m_startResponse[1] = 0x02;
  m_startResponse[2] = 0x80;
  m_startResponse[3] = 0x03;
  m_startResponse[4] = 0x00;
  m_startResponse[5] = 0xA0;
  m_startResponse[6] = 0x00;
  m_startResponse[7] = 0x10; //11?
  m_startResponse[8] = 0x16; //17?
  m_startResponse[9] = 0x0A;

  m_measurementHeader[0] = 0x73;
  m_measurementHeader[1] = 0x53;
  m_measurementHeader[2] = 0x4e;
  m_measurementHeader[3] = 0x20;
  m_measurementHeader[4] = 0x4c;
  m_measurementHeader[5] = 0x4d;
  m_measurementHeader[6] = 0x44;
  m_measurementHeader[7] = 0x73;
  m_measurementHeader[8] = 0x63;
  m_measurementHeader[9] = 0x61;
  m_measurementHeader[10] = 0x6e;
  m_measurementHeader[11] = 0x64;
  m_measurementHeader[12] = 0x61;
  m_measurementHeader[13] = 0x74;
  m_measurementHeader[14] = 0x61;
  m_measurementHeader[15] = 0x20;
  m_measurementHeader[16] = 0x31;
  m_measurementHeader[17] = 0x20;
  m_measurementHeader[18] = 0x45;
  m_measurementHeader[19] = 0x42;
  m_measurementHeader[20] = 0x38;
  m_measurementHeader[21] = 0x20;
  m_measurementHeader[22] = 0x31;
  m_measurementHeader[23] = 0x20;
  m_measurementHeader[24] = 0x30;
  m_measurementHeader[25] = 0x20;
  m_measurementHeader[26] = 0x31;
  m_measurementHeader[27] = 0x39;
  m_measurementHeader[28] = 0x45;
  m_measurementHeader[29] = 0x30;
  m_measurementHeader[30] = 0x20;
  m_measurementHeader[31] = 0x34;
  m_measurementHeader[32] = 0x20;
  m_measurementHeader[33] = 0x46;
  m_measurementHeader[34] = 0x41;
  m_measurementHeader[35] = 0x42;
  m_measurementHeader[36] = 0x36;
  m_measurementHeader[37] = 0x32;
  m_measurementHeader[38] = 0x37;
  m_measurementHeader[39] = 0x30;
  m_measurementHeader[40] = 0x32;
  m_measurementHeader[41] = 0x20;
  m_measurementHeader[42] = 0x46;
  m_measurementHeader[43] = 0x41;
  m_measurementHeader[44] = 0x42;
  m_measurementHeader[45] = 0x36;
  m_measurementHeader[46] = 0x33;
  m_measurementHeader[47] = 0x31;
  m_measurementHeader[48] = 0x45;
  m_measurementHeader[49] = 0x42;
  m_measurementHeader[50] = 0x20;
  m_measurementHeader[51] = 0x30;
  m_measurementHeader[52] = 0x20;
  m_measurementHeader[53] = 0x30;
  m_measurementHeader[54] = 0x20;
  m_measurementHeader[55] = 0x30;
  m_measurementHeader[56] = 0x20;
  m_measurementHeader[57] = 0x30;
  m_measurementHeader[58] = 0x20;
  m_measurementHeader[59] = 0x30;
  m_measurementHeader[60] = 0x20;
  m_measurementHeader[61] = 0x31;
  m_measurementHeader[62] = 0x33;
  m_measurementHeader[63] = 0x38;
  m_measurementHeader[64] = 0x38;
  m_measurementHeader[65] = 0x20;
  m_measurementHeader[66] = 0x31;
  m_measurementHeader[67] = 0x36;
  m_measurementHeader[68] = 0x38;
  m_measurementHeader[69] = 0x20;
  m_measurementHeader[70] = 0x30;
  m_measurementHeader[71] = 0x20;
  m_measurementHeader[72] = 0x32;


  m_lidarDataFile.open("lidatDataRaw.txt", ios::out);
  m_messageFile.open("message.txt", ios::out);
  m_headerFile.open("header.txt", ios::out);
}

SickStringDecoder::~SickStringDecoder()
{
    m_lidarDataFile.close();
    m_messageFile.close();
}

void SickStringDecoder::convertToDistances()
{
  uint32_t byte1;
  uint32_t byte2;
  uint32_t distance;
  double cartesian[2];

  std::vector<opendlv::model::Direction> directions; // m_directions.clear();
  std::vector<double> radii;                         // m_radii.clear();

  for (uint32_t i = 0; i < 361; i++) {
    byte1 = (int)m_measurements[i * 2];
    byte2 = (int)m_measurements[i * 2 + 1];

    if (i < 361) {
      distance = byte1 + (byte2 % 32) * 256; //Integer centimeters in local polar coordinates
    }
    else {
      distance = byte2 + (byte1 % 32) * 256; //Integer centimeters in local polar coordinates
    }

    if (distance < 7500) {                                     //We don't send max range responses
      cartesian[0] = distance * sin(M_PI * i / 360.0) / 100.0; //Local cartesian coordinates in meters (rotated coordinate system)
      cartesian[1] = distance * (-cos(M_PI * i / 360.0)) / 100.0;
      cartesian[0] += m_position[0]; //Truck cartesian coordinates
      cartesian[1] += m_position[1];

      double radius = std::sqrt(pow(cartesian[0], 2) + std::pow(cartesian[1], 2));
      float angle = static_cast<float>(std::atan2(cartesian[1], cartesian[0]));
      opendlv::model::Direction direction(angle, 0);
      directions.push_back(direction);
      radii.push_back(radius);
    }
  }

  m_latestReading.setListOfDirections(directions);
  m_latestReading.setListOfRadii(radii);
  m_latestReading.setNumberOfPoints(radii.size());

  // Distribute data.
  odcore::data::Container c(m_latestReading);
  m_conference.send(c);
}

bool SickStringDecoder::tryDecode()
{
  const uint32_t HEADER_LENGTH = 20;
  const uint32_t START_LENGTH = 6;
  const uint32_t MAX_NUMBER_OF_BYTES_PER_PAYLOAD = 1080;
  static uint32_t byteCounter = 0;
  const string s = m_buffer.str();

  if (m_header && (byteCounter < MAX_NUMBER_OF_BYTES_PER_PAYLOAD)) {
    // Store bytes in receive measurement buffer.
    uint32_t processedBytes = 0;
    while ((byteCounter < MAX_NUMBER_OF_BYTES_PER_PAYLOAD) && (processedBytes < s.size())) {
      m_measurements[byteCounter] = static_cast<char>(s.at(processedBytes));
      processedBytes++; // Bytes processed in this cycle.
      byteCounter++;    // Bytes processed in total.
    }
    m_buffer.str(m_buffer.str().substr(processedBytes));

    return true;
  }

  if (m_header && (byteCounter == MAX_NUMBER_OF_BYTES_PER_PAYLOAD)) {
    cout << "Completed payload." << endl;
    // Do ray transformation.
    convertToDistances();
    // Consumed all measurements; find next header; there should be three bytes trailing before the next sequence begins.
    m_header = false;
    return false;
  }

  // Find header.
  if (!m_header && (s.size() >= HEADER_LENGTH)) {
    m_header = true;
    for (uint32_t i = 0; (i < HEADER_LENGTH) && m_header; i++) {
      m_header &= ((int)(uint8_t)s.at(i) == (int)(uint8_t)m_measurementHeader[i]);
    }
    if (m_header) {
      cout << "Received header." << endl;
      // Remove 7 bytes.
      m_buffer.str(m_buffer.str().substr(HEADER_LENGTH));
      m_buffer.seekg(0, ios_base::end);

      // Reset byte counter for processing payload.
      byteCounter = 0;
      return true;
    }
    // Not found, try next state.
  }


  // Find start confirmation.
  if (s.size() >= START_LENGTH) {
    // Try to find start message.
    m_startConfirmed = true;
    for (uint32_t i = 0; (i < START_LENGTH) && m_startConfirmed; i++) {
      // Byte 7 and 8 might be different.
      if ((i != 7) && (i != 8)) {
        m_startConfirmed &= ((int)(uint8_t)s.at(i) == (int)(uint8_t)m_startResponse[i]);
      }
    }
    if (m_startConfirmed) {
      cout << "Received start confirmation." << endl;
      // Remove 10 bytes.
      m_buffer.str(m_buffer.str().substr(START_LENGTH));
      m_buffer.seekg(0, ios_base::end);
      return true;
    }
    // Not found, shorten buffer.
  }

  // Nothing processed.
  return false;
}

void SickStringDecoder::nextString(std::string const &a_string)
{
  //std::stringstream buffer;
  m_messageFile << a_string.size();
  m_messageFile << endl;
  for (uint32_t i = 0; i < a_string.size(); i++) {
    char c = a_string.at(i);
    m_buffer.write(&c, sizeof(char));
    //buffer.write(&c, sizeof(char));
  }
  
  string s = m_buffer.str();
  // Print out the raw lidar data
  cout << s << endl;
  // Save lidar raw data
  m_lidarDataFile << s;
  m_lidarDataFile << endl;



  // We need at least 10 bytes before we can continue.
  
  if (s.size() >= m_minimumMessageLength) {
    while (s.size() > 0) {
      if (false) {
        // If decoding succeeds, don't modify buffer but try to consume more.
      }
      else {
        // Remove first byte before continuing processing.
        m_buffer.str(m_buffer.str().substr(1));
      }
      // Check remaining length.
      s = m_buffer.str();
    }
  }
  

  // Always add at the end for more bytes to buffer.
  m_buffer.seekg(0, ios_base::end);
}

}
}
}


