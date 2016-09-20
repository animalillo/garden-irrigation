/* 
 * File:   AT24C32_64.h
 * Version: 1.0
 * Purpose: Atmel AT24C32 I2C EEPROM library for arduino
 * License: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 * Author: Marcos Zuriaga Miguel (Created on 31 of august 2016, 19:56)
 */

 /*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AT24C32_64_H
#define AT24C32_64_H

#include <Arduino.h>
#include <Wire.h>

#define AT24C_ADDRESS			0x50

class AT24C32_64 {
public:
    AT24C32_64(uint8_t address);
    AT24C32_64();
//    virtual ~AT24C32_64();
    
    void write(byte data, uint16_t address);
    void write(bool data, uint16_t address);
    void write(int data, uint16_t address);
    void write(char data, uint16_t address);
    void write(char* data, uint8_t n_chars, uint16_t address);
    void write(byte* data, uint8_t n_bytes, uint16_t address);
    
    byte readByte(uint16_t address);
    bool readBool(uint16_t address);
    int readInt(uint16_t address);
    char readChar(uint16_t address);
    void readCharArray(char* buffer, uint16_t address, uint8_t n_chars);
    void readByteArray(byte* buffer, uint16_t address, uint8_t n_bytes);
private:
    TwoWire* wire;
    uint8_t address;
    
    void read(uint8_t* buffer, uint8_t num_bytes);
    void setAddressPointer(uint16_t address);
};

#endif /* AT24C32_64_H */

