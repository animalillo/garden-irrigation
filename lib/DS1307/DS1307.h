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

#ifndef DS1307_H
#define DS1307_H

#include <Arduino.h>
#include <Wire.h>

#include "DateTime.h"

#define DS1307_ADDRESS			0x68

#define REG_DS1307_SECONDS		0x00
#define REG_DS1307_MINUTES		0x01
#define REG_DS1307_HOURS		0x02
#define REG_DS1307_DOW			0x03	// Day of week
#define REG_DS1307_DATE			0x04
#define REG_DS1307_MONTH		0x05
#define REG_DS1307_YEAR			0x06
#define REG_DS1307_CONTROL		0x07
#define REG_DS1307_RAM_USER_START 	0x08
#define REG_DS1307_RAM_USER_END 	0x3F

#define DS1307_SQUARE_WAVE_OUT_DISABLED_HIGH    0x80	
#define DS1307_SQUARE_WAVE_ENABLED		0x10
#define DS1307_SQUARE_WAVE_1HZ			0x00
#define DS1307_SQUARE_WAVE_4096HZ		0x01
#define DS1307_SQUARE_WAVE_8192HZ		0x02
#define DS1307_SQUARE_WAVE_32768HZ		0x03

class DS1307 {
public:
    DS1307();
    DS1307(TwoWire* wire);
    DateTime getTime();
    void setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t dow, uint8_t dom, uint8_t month, uint16_t year);
    void setTime(DateTime dt);
    
    void writeRAM(uint8_t address, uint8_t data);
    uint8_t readRAM(uint8_t address);

    void writeRegister(uint8_t address, uint8_t data);
    void writeRegister(uint8_t address, uint8_t data[], uint8_t count);
    uint8_t readRegister(uint8_t address);

    void setSquareWave(uint8_t mode);
    
    bool isStarted();
private:
    TwoWire* wire;
};
#endif