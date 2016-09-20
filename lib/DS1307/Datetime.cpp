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

#include "DateTime.h"

DateTime::DateTime(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t dow, uint8_t d, uint8_t m, uint16_t y){
    this->hh = hh;
    this->mm = mm;
    this->ss = ss;
    this->dow=dow;
    this->d = d;
    this->m = m;
    this->y = y;
}

String DateTime::toString(){
    String s = "";
    s = s + year() + F("-") + month() + F("-") + day() + F(" ") + hour() + F(":") + minute() + F(":") + second();
    return s;
}