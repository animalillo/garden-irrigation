/* 
 * File:   DS1307.cpp
 * Version: 1.0
 * Purpose: Date and Time simple container class
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

#ifndef DATETIME_H
#define DATETIME_H

#include <Arduino.h>

class DateTime {
public:
    DateTime(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t dow, uint8_t d, uint8_t m, uint16_t y);
    const uint16_t year()       { return  y; }
    const uint8_t month()       { return m; }
    const uint8_t day()         { return d; }
    const uint8_t dayOfWeek()   { return dow; }
    const uint8_t hour()        { return hh; }
    const uint8_t minute()      { return mm; }
    const uint8_t second()      { return ss; }
    String toString();
    void setYear(uint16_t year) { y = year; }
    void setMonth(uint8_t month) { m = month; }
    void setDay(uint8_t day) { d = day; }
    void setDayOfWeek(uint8_t dayOfWeek) { dow = dayOfWeek; }
    void setHour(uint8_t hour) { hh = hour; }
    void setMinute(uint8_t minute) { mm = minute; }
    void setSecond(uint8_t second) { ss = second; }
    
    
protected:
    uint16_t y;
    uint8_t hh, mm, ss, dow, d, m;
};

#endif /* DATETIME_H */

