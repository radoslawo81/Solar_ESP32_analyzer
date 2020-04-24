

///////////////////////////////////////////INCLUDE, DEFINE//////////////////////////////////////

#include <Wire.h>                                             //
#include <Adafruit_INA219.h>                                  //
#include <SparkFunBQ27441.h>                                  //
#include "ThingSpeak.h"
#include "secrets.h"
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 15 ///



char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
//////////////////////////////////////////KANALY THING SPEAK/////////////////////////////////
unsigned long myChannelNumber1 = SECRET_CH_ID1;              //INA219
const char * myWriteAPIKey1= SECRET_WRITE_APIKEY1;           //INA219
unsigned long myChannelNumber2 = SECRET_CH_ID2;              //BABY
const char * myWriteAPIKey2= SECRET_WRITE_APIKEY2;           //BABY
unsigned long myChannelNumber3 = SECRET_CH_ID3;              //TEMP
const char * myWriteAPIKey3= SECRET_WRITE_APIKEY3;           //TEMP
unsigned long myChannelNumber4 = SECRET_CH_ID4;              //MAIN
const char * myWriteAPIKey4= SECRET_WRITE_APIKEY4;           //MAIN

const unsigned int BATTERY_CAPACITY = 850;                    // e.g. 850mAh battery

Adafruit_INA219 ina219_A;
Adafruit_INA219 ina219_B(0x41);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices;
DeviceAddress tempDeviceAddress; 


  float t_air;
  float t_ground;
  
  float shuntvoltage_A = 0;
  float busvoltage_A = 0;
  float current_mA_A = 0;
  float loadvoltage_A = 0;
  float power_mW_A = 0;
  float shuntvoltage_B = 0;
  float busvoltage_B = 0;
  float current_mA_B = 0;
  float loadvoltage_B = 0;
  float power_mW_B = 0;

  unsigned int soc;                                            // Read state-of-charge (%)
  unsigned int volts;                                          // Read battery voltage (mV)
  int current;                                                 // Read average current (mA)
  unsigned int fullCapacity;                                   // Read full capacity (mAh)
  unsigned int capacity;                                       // Read remaining capacity (mAh)
  int power;                                                   // Read average power draw (mW)
  int health;                                                  // Read state-of-health (%)

  long cap;
  long vol;
  long soc1;
  long fullCap;






void setup() 
    {
     Serial.begin(115200);  //Initialize serial
     WiFi.mode(WIFI_STA);   
     ThingSpeak.begin(client);  // Initialize ThingSpeak
     setupBQ27441();
     ina219_A.begin();
     ina219_B.begin();
     sensors.begin();
     numberOfDevices = sensors.getDeviceCount();
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  


  Serial.print("SZUKANIE DS18B20...");
  Serial.print("ZNALEZIONO ");
  Serial.print(numberOfDevices, DEC);
  Serial.println("CZUJNIKOW.");
  for(int i=0;i<numberOfDevices; i++)
     {
      if(sensors.getAddress(tempDeviceAddress, i))
        {
        Serial.print("Found device ");
        Serial.print(i, DEC);
        Serial.print(" with address: ");
        printAddress(tempDeviceAddress);
        Serial.println();
        } 
        else 
        {
         Serial.print("Found ghost device at ");
         Serial.print(i, DEC);
         Serial.print(" but could not detect address. Check power and cabling");
        }
      }
     }









void loop() 
     {

      if(WiFi.status() != WL_CONNECTED)
        {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(SECRET_SSID);
        while(WiFi.status() != WL_CONNECTED)
             {
              WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
              Serial.print(".");
              delay(5000);     
             } 
        Serial.println("\nConnected.");
        }


        
  
  shuntvoltage_A = ina219_A.getShuntVoltage_mV();
  busvoltage_A = ina219_A.getBusVoltage_V();
  current_mA_A = ina219_A.getCurrent_mA();
  power_mW_A = ina219_A.getPower_mW();
  loadvoltage_A = busvoltage_A + (shuntvoltage_A / 1000);
  ThingSpeak.setField(1, shuntvoltage_A);
  ThingSpeak.setField(2, busvoltage_A);
  ThingSpeak.setField(3, current_mA_A);
  ThingSpeak.setField(4, loadvoltage_A);
  ThingSpeak.setField(5, power_mW_A);
  
  
  int x = ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
  if(x == 200)
    {
     Serial.println("Channel update successful.");
    }
    else
    {
     Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
     delay(60000); // Wait 20 seconds to update the channel again

 shuntvoltage_B = ina219_B.getShuntVoltage_mV();
  busvoltage_B = ina219_B.getBusVoltage_V();
  current_mA_B = ina219_B.getCurrent_mA();
  power_mW_B = ina219_B.getPower_mW();
  loadvoltage_B = busvoltage_B + (shuntvoltage_B / 1000);
  ThingSpeak.setField(1, shuntvoltage_B);
  ThingSpeak.setField(2, busvoltage_B);
  ThingSpeak.setField(3, current_mA_B);
  ThingSpeak.setField(4, loadvoltage_B);
  ThingSpeak.setField(5, power_mW_B);
  
  int W = ThingSpeak.writeFields(myChannelNumber4, myWriteAPIKey4);
  if(W == 200)
    {
     Serial.println("Channel update successful.");
    }
    else
    {
     Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
     delay(60000); // Wait 20 seconds to update the channel again












  soc = lipo.soc();  // Read state-of-charge (%)
  volts = lipo.voltage(); // Read battery voltage (mV)
  current = lipo.current(AVG); // Read average current (mA)
  fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  power = lipo.power(); // Read average power draw (mW)
  health = lipo.soh(); // Read state-of-health (%)
  
  
  soc1=soc;
  vol=volts;
  cap=capacity;
  fullCap=fullCapacity;
  
  ThingSpeak.setField(1, soc1);
  ThingSpeak.setField(2, vol);
  ThingSpeak.setField(3, current);
  ThingSpeak.setField(4, cap);
  ThingSpeak.setField(5, fullCap);
  ThingSpeak.setField(6, power);
  ThingSpeak.setField(7, health);
  
  // write to the ThingSpeak channel
  int y = ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);
  if(y == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(y));
  }
  delay(60000); // Wait 20 seconds to update the channel again

 
  
   sensors.requestTemperatures(); // Send the command to get temperatures
   sensors.getAddress(tempDeviceAddress, 0); 
   t_air=sensors.getTempC(tempDeviceAddress);
   sensors.getAddress(tempDeviceAddress, 1);
   t_ground=sensors.getTempC(tempDeviceAddress);
   Serial.println(t_air);
   Serial.println(t_ground);
  
  ThingSpeak.setField(1, t_air);
  ThingSpeak.setField(2, t_ground);

  int z = ThingSpeak.writeFields(myChannelNumber3, myWriteAPIKey3);
  if(z == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(y));
  }
  delay(60000); // Wait 20 seconds to update the channel again
}












void setupBQ27441(void)
     {
      if (!lipo.begin()) // begin() will return true if communication is successful
         {
          Serial.println("Error: Unable to communicate with BQ27441.");
          Serial.println("  Check wiring and try again.");
          Serial.println("  (Battery must be plugged into Battery Babysitter!)");
          while (1) ;
         }
     Serial.println("Connected to BQ27441!");
     lipo.setCapacity(BATTERY_CAPACITY);
     }



    
void printAddress(DeviceAddress deviceAddress) 
    {
     for (uint8_t i = 0; i < 8; i++)
        {
         if (deviceAddress[i] < 16) Serial.print("0");
            Serial.print(deviceAddress[i], HEX);
        }
     }
