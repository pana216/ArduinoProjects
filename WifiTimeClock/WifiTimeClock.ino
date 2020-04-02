#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>

// Wifi Connection Data
const char* ssid     = "FRITZ!Box 7362 SL";
const char* password = "94571092175627330410";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Display Object with the specific pins
#define CLK 22
#define DIO 23
TM1637Display display(CLK, DIO);

// Variables to save date and time
String formattedDate;
String timeStamp;
unsigned long timeNow = 0;
unsigned long timeLast = 0;
int hours;
int minutes;
int seconds;
int value = 1244;
uint8_t colonOn = 0x80 | display.encodeDigit((value / 100)%10);
uint8_t colonOff = 0x00 | display.encodeDigit((value / 100)%10);


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Connect to the specific Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example: GMT +1 = 3600; GMT +8 = 28800; GMT -1 = -3600; GMT 0 = 0
  timeClient.setTimeOffset(3600); //TODO: In the future get location via wifi and automaticaly set the offset
// Display coonfigurations
  display.setBrightness(8);
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setSegments(data);
  timeString();
  
  // Add the hours + minutes to the display
  display.showNumberDec(hours, true, 2, 0);
  display.showNumberDec(minutes, true, 2, 2);
  
  WiFi.mode(WIFI_OFF);
  }

void timeString(){
   while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract time /TODO: get summer-/wintertime automatic
  int splitT = formattedDate.indexOf("T");
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);
  hours = 1 + (timeStamp.substring(0,2).toInt()); // +1 because of the summer-/wintertime
  minutes = timeStamp.substring(3,5).toInt();
  seconds = timeStamp.substring(6, timeStamp.length()).toInt();
}

//   display.setSegments(&colonOn, 1, 1); // Turns the colon on
 //   display.setSegments(&colonOff, 1, 1); // Turns the colon off
void loop() {
    display.setSegments(&colonOn, 1, 1); // Turns the colon on
    timeNow = millis()/1000;
    seconds = timeNow - timeLast;
    Serial.print("TimeNow: ");
    Serial.print(timeNow);
    Serial.println(" ");
    Serial.print("TimeLast: ");
    Serial.println(timeLast);
    
    if (seconds == 60){
 
      timeLast = timeNow;
      minutes += 1;
      display.showNumberDec(minutes, true, 2, 2);
    }

    if (minutes == 60) {
        minutes = 0;
        hours += 1;
        display.showNumberDec(minutes, true, 2, 2);
        display.showNumberDec(hours, true, 2, 0);
    }

    if (hours == 24){
        hours = 0;
        //ESP.restart();
    }

    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.print(seconds);
    Serial.println(" ");

    
    delay(500);
}

 
          
