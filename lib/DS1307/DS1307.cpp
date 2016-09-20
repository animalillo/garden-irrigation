/* 
 * File:   DS1307.cpp
 * Version: 1.0
 * Purpose: DS1307 I2C RTC library for arduino
 * License: GPL v3 (http://www.gnu.org/licenses/gpl.html)
 * Author: Marcos Zuriaga Miguel (Created on 04 of July 2016)
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

#include "DS1307.h"

static uint8_t fromBCD (byte val) {
  //return (value >> 4) * 10 + (value & 0x0F);
    //return val - 6 * (val >> 4);
    return ((val >> 4)*10) + (val & 0x0F);
}

static uint8_t toBCD(uint8_t val) {
//  return ((val/10*16) + (val%10));
//    return val + 6 * (val / 10);
    return ((val/10) << 4) + val%10;
}

DS1307::DS1307() {
    wire = &Wire;
}

DS1307::DS1307(TwoWire* wire) {
    this->wire = wire;
}

void DS1307::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t dow, uint8_t dom, uint8_t month, uint16_t year) {
    uint8_t dt[7];
    dt[0] = toBCD(second);
    dt[1] = toBCD(minute);
    dt[2] = toBCD(hour);
    dt[3] = toBCD(dow);
    dt[4] = toBCD(dom);
    dt[5] = toBCD(month);
    dt[6] = toBCD(year - 2000);
    this->writeRegister(REG_DS1307_SECONDS, dt, 7);
}

void DS1307::setTime(DateTime dt) {
    setTime(dt.hour(), dt.minute(), dt.second(), dt.dayOfWeek(), dt.day(), dt.month(), dt.year());
}


DateTime DS1307::getTime() {
    uint8_t hh,mm,ss,dow,d,m;
    uint16_t y;
    wire->beginTransmission(DS1307_ADDRESS);
    wire->write(REG_DS1307_SECONDS);
    wire->endTransmission();
    
    wire->requestFrom(DS1307_ADDRESS, 7);
    ss = fromBCD(wire->read() & 0x7F);
    mm = fromBCD(wire->read());
    hh = fromBCD(wire->read());
    dow = fromBCD(wire->read());
    d = fromBCD(wire->read());
    m = fromBCD(wire->read());
    y = fromBCD(wire->read()) + 2000;
    
    return DateTime(hh,mm,ss,dow,d,m,y);
}


void DS1307::writeRegister(uint8_t address, uint8_t data){
	wire->beginTransmission(DS1307_ADDRESS);
	wire->write(address);
	wire->write(data);
        wire->endTransmission();
}

void DS1307::writeRegister(uint8_t address, uint8_t data[], uint8_t count){
    wire->beginTransmission(DS1307_ADDRESS);
    wire->write(address);
    
    for (int i = 0; i < count; i ++){
        wire->write(data[i]);
    }
    
    wire->endTransmission();
}

uint8_t DS1307::readRegister(uint8_t address){
    wire->beginTransmission(DS1307_ADDRESS);
    wire->write(address);
    wire->endTransmission();
    wire->requestFrom(DS1307_ADDRESS, 1);
    return wire->read();
}

void DS1307::writeRAM(uint8_t address, uint8_t data){
    writeRegister(REG_DS1307_RAM_USER_START + address, data);
}

uint8_t DS1307::readRAM(uint8_t address){
    return readRegister((uint8_t) (REG_DS1307_RAM_USER_START + address));
}

bool DS1307::isStarted() {
    uint8_t s = readRegister(REG_DS1307_SECONDS);
    
    return !(s >> 7);
}