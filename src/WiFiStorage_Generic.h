/****************************************************************************************************************************
  WiFiStorage_Generic.h - Library for Arduino WifiNINA module/shield.

  Based on and modified from WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  to support nRF52, SAMD21/SAMD51, STM32F/L/H/G/WB/MP1, Teensy, etc. boards besides Nano-33 IoT, MKRWIFI1010, MKRVIDOR400, etc.
  
  Built by Khoi Hoang https://github.com/khoih-prog/WiFiNINA_Generic
  Licensed under MIT license

  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Version: 1.6.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.5.0   K Hoang      27/03/2020 Initial coding to support other boards besides Nano-33 IoT, MKRWIFI1010, MKRVIDOR4000, etc.
                                  such as Arduino Mega, Teensy, SAMD21, SAMD51, STM32, etc
  1.5.1   K Hoang      22/04/2020 Add support to nRF52 boards, such as AdaFruit Feather nRF52832, nRF52840 Express, BlueFruit Sense,
                                  Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, etc.
  1.5.2   K Hoang      09/05/2020 Port FirmwareUpdater to permit nRF52, Teensy, SAMD21, SAMD51, etc. boards to update WiFiNINA
                                  W101/W102 firmware and SSL certs on IDE. Update default pin-outs.
  1.5.3   K Hoang      14/07/2020 Add function to support new WebSockets2_Generic Library
  1.6.0   K Hoang      19/07/2020 Sync with Aruino WiFiNINA Library v1.6.0 (new Firmware 1.4.0 and WiFiStorage)
  1.6.1   K Hoang      24/07/2020 Add support to all STM32F/L/H/G/WB/MP1 and Seeeduino SAMD21/SAMD51 boards 
  1.6.2   K Hoang      28/07/2020 Fix WiFiStorage bug from v1.6.0   
 *****************************************************************************************************************************/

#ifndef WiFiStorage_Generic_h
#define WiFiStorage_Generic_h

#include "utility/wifi_drv.h"

class WiFiStorageFile;

class WiFiStorageClass
{
  public:
    static bool begin();

    static WiFiStorageFile open(const char *filename);
    static WiFiStorageFile open(String filename);

    static bool exists(const char *filename) 
    {
      uint32_t len;
      return (WiFiDrv::existsFile(filename, strlen(filename), &len) > 0);
    }
    
    static bool exists(const char *filename, uint32_t* len) 
    {
      return (WiFiDrv::existsFile(filename, strlen(filename), len) > 0);
    }
    
    static bool remove(const char *filename) 
    {
      WiFiDrv::deleteFile(filename, strlen(filename));
      return true;
    }
    
    static bool rename(const char * old_file_name, const char * new_file_name) 
    {
      return (WiFiDrv::renameFile(old_file_name, strlen(old_file_name), new_file_name, strlen(new_file_name)) == 0);
    }
    
    static bool read(const char *filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) 
    {
      WiFiDrv::readFile(filename, strlen(filename), offset, buffer, buffer_len);
      return true;
    }
    
    static bool write(const char *filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) 
    {
      WiFiDrv::writeFile(filename, strlen(filename), offset, buffer, buffer_len);
      return true;
    }
    
    static bool download(const char* url, const char *filename) 
    {
      WiFiDrv::downloadFile(url, strlen(url), filename, strlen(filename));
      return true;
    }

    static bool remove(String filename) 
    {
      return remove(filename.c_str());
    }
    
    static bool rename(String old_file_name, String new_file_name) 
    {
      return rename(old_file_name.c_str(), new_file_name.c_str());
    }
    
    static bool read(String filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) 
    {
      return read(filename.c_str(), offset, buffer, buffer_len);
    }
    
    static bool write(String filename, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) 
    {
      return write(filename.c_str(), offset, buffer, buffer_len);
    }
    
    static bool download(String url, String filename) 
    {
      return download(url.c_str(), filename.c_str());
    }
};

extern WiFiStorageClass WiFiStorage;


class WiFiStorageFile
{
  public:
    constexpr WiFiStorageFile(const char* _filename) : filename(_filename) { }

    operator bool() 
    {
      return WiFiStorage.exists(filename, &length);
    }
    
    uint32_t read(void *buf, uint32_t rdlen) {
      
      if (offset + rdlen > length) 
      {
        if (offset >= length) 
          return 0;
          
        rdlen = length - offset;
      }
      
      WiFiStorage.read(filename, offset, (uint8_t*)buf, rdlen);
      offset += rdlen;
      
      return rdlen;
    }
    
    uint32_t write(const void *buf, uint32_t wrlen) 
    {
      WiFiStorage.write(filename, offset, (uint8_t*)buf, wrlen);
      offset += wrlen;
      
      return wrlen;
    }
    
    void seek(uint32_t n) 
    {
      offset = n;
    }
    
    uint32_t position() 
    {
      return offset;
    }
    
    uint32_t size() 
    {
      WiFiStorage.exists(filename, &length);
      return length;
    }
    
    uint32_t available() 
    {
      WiFiStorage.exists(filename, &length);
      return length - offset;
    }
    
    void erase() 
    {
      offset = 0;
      WiFiStorage.remove(filename);
    }
    
    void flush();
    
    void close() 
    {
      offset = 0;
    }
    
  protected:
    friend class WiFiStorageClass;
    uint32_t offset = 0;
    uint32_t length = 0;
    const char* filename;
};

#endif    // WiFiStorage_Generic_h
