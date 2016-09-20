/* 
 * File:   AT24C32_64.cpp
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

#include "AT24C32_64.h"

/**
 * Atmel AT24C has a configurable part of the address,
 * so you can easily use this class to address them by
 * setting it's address part here, the rest of the address
 * will be automatically filled.
 * @param address
 */
AT24C32_64::AT24C32_64(uint8_t address) {
    this->address = AT24C_ADDRESS | address;
    wire = &Wire;
}

AT24C32_64::AT24C32_64() { 
    this->address = AT24C_ADDRESS;
    wire = &Wire;
}

void AT24C32_64::setAddressPointer(uint16_t address) {
    wire->beginTransmission(this->address);
    wire->write((uint8_t) address >> 8);
    wire->write((uint8_t) address);
}

void AT24C32_64::read(uint8_t* buffer, uint8_t num_bytes = 1) {
    wire->requestFrom(address, num_bytes);
    wire->readBytes(buffer, (size_t) num_bytes);
}

byte AT24C32_64::readByte(uint16_t address) {
    setAddressPointer(address);
    wire->endTransmission();
    
    uint8_t data = 0;
    read(&data);
    
    
    return data;
}

bool AT24C32_64::readBool(uint16_t address) {
    return (bool) readByte(address);
}

char AT24C32_64::readChar(uint16_t address) {
    return (char) readByte(address);
}

void AT24C32_64::readCharArray(char* buffer, uint16_t address, uint8_t n_chars) {
    setAddressPointer(address);
    wire->endTransmission();
    
    read((uint8_t *)buffer, n_chars);
}

void AT24C32_64::readByteArray(byte* buffer, uint16_t address, uint8_t n_bytes) {
    setAddressPointer(address);
    wire->endTransmission();
    
    read(buffer, n_bytes);
}

int AT24C32_64::readInt(uint16_t address) {
    uint8_t buff[2] = {0,0};
    
    setAddressPointer(address);
    wire->endTransmission();
    
    read(buff,2);
    
    int data = 0;
    data = data | buff[0] << 8;
    data = data | buff[1];
    
    return data;
}

void AT24C32_64::write(byte data, uint16_t address) {
    setAddressPointer(address);
    wire->write(data);
    wire->endTransmission();
}

void AT24C32_64::write(bool data, uint16_t address) {
    write((byte)data, address);
}

void AT24C32_64::write(char* data, uint8_t n_chars, uint16_t address) {
    setAddressPointer(address);
    
    for (uint8_t i = 0; i < n_chars; i++){
        wire->write(data[i]);
    }
    wire->endTransmission();
}

void AT24C32_64::write(byte* data, uint8_t n_chars, uint16_t address) {
    setAddressPointer(address);
    
    for (uint8_t i = 0; i < n_chars; i++){
        Serial.println(data[i]);
        wire->write(data[i]);
    }
    
    wire->endTransmission();
}
