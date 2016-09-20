/* 
 * File:   DS1307.cpp
 * Version: 1.0
 * Purpose: Controlling relays, making them interact with time and 
 * log some sensor data while exposing some configuration and functions
 * trough the network.
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

#include <Wire.h>
#include <SD.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "lib/DHT11/DHT11.h"
#include "lib/DS1307/DS1307.h"
#include "lib/AT24C32_64/AT24C32_64.h"

#define DHT11_PIN 7
#define SW1 9
#define SW2 8

#define ALARM1_START_ADDRESS    0
#define ALARM2_START_ADDRESS    3
#define ALARM3_START_ADDRESS    6
#define ALARM4_START_ADDRESS    9

#define ALARM5_START_ADDRESS    12
#define ALARM6_START_ADDRESS    15
#define ALARM7_START_ADDRESS    18
#define ALARM8_START_ADDRESS    21

#define ALARM1_STOP_ADDRESS     24
#define ALARM2_STOP_ADDRESS     27
#define ALARM3_STOP_ADDRESS     30
#define ALARM4_STOP_ADDRESS     33

#define ALARM5_STOP_ADDRESS     36
#define ALARM6_STOP_ADDRESS     39
#define ALARM7_STOP_ADDRESS     42
#define ALARM8_STOP_ADDRESS     45

#define FORCE_SW1_OFF_ADDRESS   100
#define FORCE_SW2_OFF_ADDRESS   101

DS1307 rtc;
AT24C32_64 rom;

dht11 DHT11;
EthernetServer server(80);
unsigned long last_log = 0;
unsigned long last_tick = 0;
bool force_sw1_on = false;
bool force_sw2_on = false;
bool force_sw1_off = false;
bool force_sw2_off = false;

void setup() {
  Wire.begin();

  Serial.begin(9600);
  
  if (!rtc.isStarted()){
      Serial.println(F("setting up RTC for first boot"));
      startClock();
  }

  if (!SD.begin(4)) {
    Serial.println(F("initialization failed!"));
  }
  
  force_sw1_off = rom.readBool(FORCE_SW1_OFF_ADDRESS);
  force_sw2_off = rom.readBool(FORCE_SW2_OFF_ADDRESS);
  
  pinMode(SW1, OUTPUT);
  pinMode(SW2, OUTPUT);
  
  setupEthernet();
  
  Serial.println(F("ready"));
}

void loop() {
  DateTime t = rtc.getTime();
  while (DHT11.read(DHT11_PIN) != 0) delay(50);

//  DHT11.read(DHT11_PIN);
//  Serial.print(F("humidity:"));
//  Serial.println(DHT11.humidity);
//  Serial.print(F("temperature"));
//  Serial.println(DHT11.temperature);
  
  /*Serial.print(t.hour());Serial.print(":");Serial.print(t.minute());Serial.print(":");Serial.println(t.second());
  Serial.print("Day of the week: "); Serial.println(t.dayOfWeek());
  Serial.print(t.year());Serial.print("-");Serial.print(t.month());Serial.print("-");Serial.println(t.day());
*/
  EthernetClient client = server.available();
  if (client){
    // an http request ends with a blank line
    while (client.connected()) {
      if (client.available()) {
        String method = readLine(&client);
        String line = "";
        while (line.charAt(line.length()-1) != '\n') {
          line = readLine(&client);
          Serial.println(line.c_str());
        }

        Serial.println(method.c_str());
        if (method.indexOf(F("/sw1")) > 0){
          if (method.indexOf(F("/on")) > 0) {
            force_sw1_on = true;
            force_sw1_off = false;
          }

          if (method.indexOf(F("/off")) > 0){
            force_sw1_on = false;
            force_sw1_off = false;

            if (method.indexOf(F("/force")) > 0){
                force_sw1_off = true;
            }
          }
          
          rom.write(force_sw1_off, FORCE_SW1_OFF_ADDRESS);
          
          http_ok_html(&client);

          client.println(method.c_str());
        }
        else if (method.indexOf(F("/sw2")) > 0){
          if (method.indexOf(F("/on")) > 0) {
            force_sw2_on = true;
            force_sw2_off = false;
          }

          if (method.indexOf(F("/off")) > 0){
            force_sw2_on = false;
            force_sw2_off = false;
            
            if (method.indexOf(F("/force")) > 0){
                force_sw2_off = true;
            }
            
          }
          
          rom.write(force_sw2_off, FORCE_SW2_OFF_ADDRESS);
            
          http_ok_html(&client);

          client.println(method.c_str());
        }
        else if (method.indexOf(F("/log")) > 0){
          http_ok_text(&client);
          
          File current_file = SD.open(F("/log/data.log"));
          dump_logs(&client, &current_file);
          current_file.close();
          SD.remove(F("/log/data.log"));
//          printDirectory(f, 0);
        }
        else if (method.indexOf(F("getSensors")) > 0){
            http_ok_text(&client);
            
            client.print(F("H:")); client.print(DHT11.humidity); client.print(F(";")); 
            client.print(F("T:")); client.print(DHT11.temperature); client.print(F(";"));
            client.print(F("SW1:"));client.print(checkSW1(&t));client.print(F(";"));
            client.print(F("SW2:")); client.print(checkSW2(&t)); client.println(F(";"));
        }
        else if (method.indexOf(F("setTime")) > 0){
            http_ok_html(&client);
            
            t = extractUrlDate(&method, method.indexOf(F("setTime")));
            rtc.setTime(t);
        }
        else if (method.indexOf(F("getTimes"))){
            http_ok_text(&client);
            
            client.print(F("Current:")); client.print(t.toString().c_str()); client.println(F(";"));
            
            byte start[3] = {0,0,0};
            byte stop[3] = {0,0,0};

            for (uint8_t i = 0; i < 8; i++){
                rom.readByteArray((byte*)&start, ALARM1_START_ADDRESS + i*3, 3);
                rom.readByteArray((byte*)&stop, ALARM1_STOP_ADDRESS   + i*3, 3);
                
                client.print(F("Alarm")); client.print(i +1); client.print(F(":"));
                for (uint8_t x = 0; x < 3; x++){
                    client.print(start[x]); //client.print(F(":")); client.print(start[1]); client.print(F(":")); client.print(start[2]);
                    if (x < 2) client.print(F(":"));
                }
                client.print(F(","));
                for (uint8_t x = 0; x < 3; x++){
                    client.print(stop[x]); //client.print(stop[0]); client.print(F(":")); client.print(stop[1]); client.print(F(":")); client.print(stop[2]);
                    if (x < 2) client.print(F(":"));
                }
                
                client.println(F(";"));
            }
        }
        else if (method.indexOf(F("setAlarm")) > 0){
            http_ok_html(&client);
            
            if (method.indexOf(F("start")) > 0){
                int tmp = method.indexOf(F("/start"));
                char c[1] = {' '};
                
                t = extractUrlDate(&method, tmp+1);
                
                c[0] = method[tmp -1];
                tmp = atoi(c);
                
                tmp = atoi(c);
                
                setAlarm(&t, ALARM1_START_ADDRESS + 3*(tmp-1));
            }
            
            if (method.indexOf(F("stop")) > 0){
                int tmp = method.indexOf(F("/stop"));
                char c[2] = {' '};
                
                t = extractUrlDate(&method, tmp+1);
                c[0] = method[tmp -1];

                tmp = atoi(c);

                setAlarm(&t, ALARM1_STOP_ADDRESS + 3*(tmp-1));
            }
        }
        else {
            http_ok_html(&client);
        }
        
        
          delay(1);
          client.stop();
      }
    }
  }
  
  t = rtc.getTime();
  
  if (millis() - last_tick >= 1000){
//      Serial.print(t.hour());Serial.print(F(":"));Serial.print(t.minute());Serial.print(F(":"));Serial.println(t.second());
//      if(force_sw1_off) Serial.println(F("SW1 off forced"));
      
      switch1(checkSW1(&t));
      switch2(checkSW2(&t));
      
      last_tick = millis();
  }

  // Log each minute
  if (millis() - last_log  >= 60000){

    String dir = F("/log");
    if (SD.mkdir(dir)){
      File current_file;
      dir += F("/data.log");
      current_file = SD.open(dir, FILE_WRITE);

      current_file.print(F("<")); current_file.print(t.toString().c_str()); current_file.print(F(">")); 
      current_file.print(F("H:")); current_file.print(DHT11.humidity); current_file.print(F(";")); 
      current_file.print(F("T:")); current_file.print(DHT11.temperature); current_file.println(F(";"));
      current_file.close();
      
      last_log = millis();
    }
  }
}

bool checkSW1(DateTime* t) {
    return (!force_sw1_off && fireAlarm(t, ALARM1_START_ADDRESS, ALARM1_STOP_ADDRESS, 4)) || force_sw1_on;
}
bool checkSW2(DateTime* t) {
    return (!force_sw2_off && fireAlarm(t, ALARM5_START_ADDRESS, ALARM5_STOP_ADDRESS, 4)) || force_sw2_on;
}

void http_ok(EthernetClient* client){
    client->println(F("HTTP/1.1 200 OK"));
}

void http_ok_html(EthernetClient* client){
    http_ok(client);
    client->println(F("Content-Type: text/html"));
    client->println();
}

void http_ok_text(EthernetClient* client){
    http_ok(client);
    client->println(F("Content-Type: text/plain"));
    client->println();
}

void dump_logs(EthernetClient* client, File* cf){
    while (true) {
       char c = cf->read();
       if (c == -1) break;
       client->print(c);
    }
}

void setAlarm(DateTime* t, uint16_t address){
    byte time[3] = {t->hour(), t->minute(), t->second()};
//    Serial.print(F("Setting alarm: ")); Serial.println(address);
//    Serial.print(time[0]); Serial.print(F(":")); Serial.print(time[1]); Serial.print(F(":")); Serial.println(time[2]);
    rom.write((byte *)&time, (uint8_t)3, address);
}

bool fireAlarm(DateTime* t, uint16_t alarm_start_address, uint16_t alarm_end_address, uint8_t hops){
    byte start[3] = {0,0,0};
    byte stop[3] = {0,0,0};
    
    for (uint8_t i = 0; i < hops; i++){
        rom.readByteArray((byte*)&start, alarm_start_address + i*3, 3);
        rom.readByteArray((byte*)&stop, alarm_end_address    + i*3, 3);
        
        if (t->hour() >= start[0] && t->hour() < stop[0]){
            return true;
        }
        else if (t->hour() == stop[0]){
            if (t->minute() < stop[1]){
                return true;
            }
            else if(t->minute() == stop[1]){
                if (t->second() <= stop[2]) return true;
            }
        }
            
            
    }
//        Serial.print("Alarm "); Serial.print(ALARM1_START_ADDRESS + i*3); Serial.print("+"); Serial.print(ALARM1_STOP_ADDRESS  + i*3); Serial.println(" Fired");
//        Serial.print(start[0]); Serial.print(F(":")); Serial.print(start[1]); Serial.print(F(":")); Serial.println(start[2]);
//        Serial.print(stop[0]); Serial.print(F(":")); Serial.print(stop[1]); Serial.print(F(":")); Serial.println(stop[2]);
    return false;
}

DateTime extractUrlDate(String* date_url, int start){
    start = date_url->indexOf('/', start + 1) + 1;
    
    uint8_t ct = 0;
    uint8_t type = 0;
    uint16_t tmp_number = 0;
    char buff[4] = {0,0,0,0};
    char c = ' ';
    
    DateTime dt = DateTime(18, 30, 40, 6, 28, 8, 2016);
    while (true) {
        if (type > 6) break;
        
        c = date_url->charAt(start);
        if (c == '/') {
            tmp_number = atoi(buff);
            
            switch(type){
                case 0:
                    dt.setYear(tmp_number);
                    break;
                case 1:
                    dt.setMonth(tmp_number);
                    break;
                case 2:
                    dt.setDay(tmp_number);
                    break;
                case 3:
                    dt.setDayOfWeek(tmp_number);
                    break;
                case 4:
                    dt.setHour(tmp_number);
                    break;
                case 5:
                    dt.setMinute(tmp_number);
                    break;
                case 6:
                    dt.setSecond(tmp_number);
                    break;
            }
            type ++;
            ct = 0;
            tmp_number = 0;
            for (byte i = 0; i < 4; i++) buff[i] = 0;
        }
        else {
            buff[ct] = c;
            ct ++;
        }
        
        start ++;
        //if ()
    }
    
    return dt;
}

void switch1(bool status){
  digitalWrite(SW1, (status) ? HIGH : LOW);
}

void switch2(bool status){
    digitalWrite(SW2, status);
}

String readLine(EthernetClient* client){
  String rturn = "";
  while(client->connected()){
    if (client->available()){
      char c = client->read();
      rturn += c;
      if (c == '\n'){
        return rturn;
      }
    }
  }
  return rturn;
}

void startClock(){
    DateTime dt = DateTime(18, 30, 40, 6, 28, 8, 2016);
    rtc.setTime(dt);
}

void setupEthernet(){
  if (SD.exists(F("/NET.CFG"))){
    char c = ' ';
    
    byte mac[6] = {0,0,0,0,0,0};
    byte ip[4] = {0,0,0,0}, netmask[4] = {0,0,0,0}, gateway[4] = {0,0,0,0}, dns[4] = {0,0,0,0};
    
    File current_file = SD.open(F("/NET.CFG"), FILE_READ);

    // Order of the calls is important!
    getMac(&current_file, mac);
    getIP(&current_file, ip);
    getIP(&current_file, netmask);
    getIP(&current_file, gateway);
    getIP(&current_file, dns);

//    Serial.print(F("MAC: "));Serial.print(mac[0], HEX);Serial.print(":");Serial.print(mac[1], HEX);Serial.print(":");Serial.print(mac[2], HEX);Serial.print(":");Serial.print(mac[3], HEX);Serial.print(":");Serial.print(mac[4], HEX);Serial.print(":");Serial.println(mac[5], HEX);
//    Serial.print(F("IP: "));Serial.print(ip[0]);Serial.print(".");Serial.print(ip[1]);Serial.print(".");Serial.print(ip[2]);Serial.print(".");Serial.println(ip[3]);
//    Serial.print(F("netmask: "));Serial.print(netmask[0]);Serial.print(".");Serial.print(netmask[1]);Serial.print(".");Serial.print(netmask[2]);Serial.print(".");Serial.println(netmask[3]);
//    Serial.print(F("dns: "));Serial.print(dns[0]);Serial.print(".");Serial.print(dns[1]);Serial.print(".");Serial.print(dns[2]);Serial.print(".");Serial.println(dns[3]);
//    Serial.print(F("gateway: "));Serial.print(gateway[0]);Serial.print(".");Serial.print(gateway[1]);Serial.print(".");Serial.print(gateway[2]);Serial.print(".");Serial.println(gateway[3]);
    
    Ethernet.begin(mac, ip, dns, gateway, netmask);
    server.begin();
  }
  else {
    //Serial.println("file not found"); Serial.println(F("/NET.CFG"));
    // TODO: store network config and load it from rom as fallback?
  }
}

void getIP(File* cf, byte* ip){
  char c = ' ';
  char buff[3] = {0,0,0};
  
  uint8_t ct = 0;
  uint8_t ct_data = 0;
  while (c != '\n') {
    c = cf->read();

    if (ct_data < 4) {
      // If it's not a net separator (.) or a newline, set the buffer
      if (c != '.' && c != '\n'){
        buff[ct] = c;
        ct++;
      }
      else {
        // Skip this byte and set counters
        ip[ct_data] = atoi(buff);

        ct = 0;
        ct_data++;

        for (byte i = 0; i < 3; i++) buff[i] = 0;
      }
    }
    else {
      cf->read();
    }
  }
}

void getMac(File* cf, byte* mac){
  char c = ' ';
  
  uint8_t ct = 2, ct_data = 0;
  
  while (c != '\n'){
    c = cf->read();
    
    if (ct_data < 6) {
      // If it's not a mac separator (:) or a newline, calculate the mac byte
      if (c != ':' && c != '\n'){
        ct--;
        mac[ct_data] |= getVal(c) << ct*4;
      }
      else {
        // Skip this byte and set counters
        ct = 2;
        ct_data++;
      }
    }
    else{
      cf->read();
    }
  }
}

byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c - '0');
   else
     return (byte)(c-'A'+10);
}