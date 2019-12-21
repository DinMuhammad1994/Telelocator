#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

int PulseSensorPurplePin = 0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 600;            // Determine which Signal to "count as a beat", and which to ingore.




#define DHTPIN D3 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define OLED_RESET LED_BUILTIN //4
Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};


//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif


TinyGPSPlus gps;  // The TinyGPS++ object
//gps tx rx
SoftwareSerial ss(D6, D5); // The serial connection to the GPS device

//const char* ssid = "Telelocator";
//const char* password = "12345678";

const char* ssid = "Projexels";
const char* password = "Qwerty123";


float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str;
String temp_str , hum_str,pulse_str;
int pm;








WiFiServer server(80);
void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  dht.begin();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("TELELOCATOR");
  display.setTextSize(1);
  display.setCursor(20, 10);
  display.print("FOR ELDERLY");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("IP:");
  
  display.setCursor(20, 0);
  display.print(WiFi.localIP());
  display.display();
  delay(5000);
  display.clearDisplay();


}

void loop()
{




  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
      }

      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        //  minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ; // to add +5 for Pakistan
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";

      }

      Serial.print("latitude");
      Serial.print(latitude, 6);
      Serial.print(", longitude");
      Serial.println(longitude, 6);
 
      Serial.print("Date: ");
      Serial.print(date);
      Serial.print("/");
      Serial.print(month);
      Serial.print("/");
      Serial.println(year);

      Serial.print("time: ");
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.println(second);

      
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  temp_str = String (t) + String ("  *C");
  hum_str =  String (h) + String ("  %");
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  
      
//dht11-------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("*WEATHER*");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println("T: ");
  display.setCursor(30, 25);
  display.println(t);
  display.setCursor(95, 25);
  display.println("C");
  
  display.setTextSize(2);
  display.setCursor(0, 50);
  display.println("H: ");
  display.setCursor(30, 50);
  display.println(h);
  display.setCursor(95, 50);
  display.println("%");
  
  display.display();
  delay(1000);

  //---------------------------------
  //GPS------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("*LOCATION*");
  
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println("LAT :");
  display.setTextSize(1);
  display.setCursor(60, 30);
  display.println(latitude,6);

  display.setTextSize(2);
  display.setCursor(0, 50);
  display.println("LONG:");
  display.setTextSize(1);
  display.setCursor(60, 55);
  display.println(longitude, 6);

  display.display();
  delay(1000);

  //time----------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.println("*-TIME-*");
  
  display.setTextSize(2);
  display.setCursor(30, 25);
  display.println(hour);
  display.setCursor(40, 25);
  display.println(" : ");
  display.setCursor(70, 25);
  display.println(minute);
 
  if (pm == 1)
  {
   display.setCursor(95, 25);
   display.println("PM");
  }
  else
  {
  display.setCursor(95, 25);
  display.println("AM");  
  }
  display.display();
  delay(1000);



  //date----------------------
  display.clearDisplay(); 
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.println("*-DATE-*");
  
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.println(date);
  display.setCursor(30, 25);
  display.println(":");
  display.setCursor(40, 25);
  display.println(month);
  display.setCursor(60, 25);
  display.println(":");
  display.setCursor(80, 25);
  display.println(year);


  
  display.display();
  delay(1000);
  display.clearDisplay();


   Signal = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
   Serial.print("Pulse signal");                                           // Assign this value to the "Signal" variable
   Serial.println(Signal);                    // Send the Signal value to Serial Plotter.


   if(Signal > Threshold){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
     pulse_str = String ("HIGH");
   } else {
     pulse_str = String ("NORMAL");                //  Else, the sigal must be below "550", so "turn-off" this LED.
   }
  

  display.setTextSize(2);
  display.setCursor(10, 0);
  display.println("*-PULSE-*");
  
  display.setTextSize(2);
  display.setCursor(30, 30);
  display.println(pulse_str);
  
 
  display.display();
  delay(1000);
  display.clearDisplay();
  
    }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td {border: 1px solid black;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> Telelocator For Elderly</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td></tr> <tr> <th>Temperature</th> <td ALIGN=CENTER >";
  s += temp_str;
  s += "</td></tr> <tr> <th>Humidity</th> <td ALIGN=CENTER >";
  s += hum_str;
   s += "</td></tr> <tr> <th>Pulse</th> <td ALIGN=CENTER >";
  s += pulse_str;
  s += "</td>  </tr> </table> ";


  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += lat_str;
    s += "+";
    s += lng_str;
    s += """ target=""_top"">Click here!</a> To check the location in Google maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

}
