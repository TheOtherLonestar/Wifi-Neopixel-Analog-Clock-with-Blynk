/************************************
 * WiFi Neopixel Analog Clock project with Alarm all controlled via the Blynk app
 * Created on live stream twitch.tv/theotherlonestar
 */

/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#include "Adafruit_NeoPixel.h"
#define BLYNK_PRINT Serial
#include"pitches.h"
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


#define snooze D1
#define speaker D8

// NeoPixel Settings
const int NEOPIXEL_DATA_PIN = D6;       // WeMos digital pin D6
const int NEO_NUM_PIXELS = 12 + 16;     // two rings
const int BRIGHTNESS = 100;              // experiment with this
int BRIGHTNESSTIME;
// the minute ring is daisy-chained to the hour ring, with indexes of 12-36
const int MINUTE_PIXEL_OFFSET = 12;
int R; //global red amount
int G; //global green amount
int B; //global blue amount
int CTime; //variable for color time. Conversion of time of day into a 0-12 scale based on day night cycle 8am-8pm = CTime of 0-12 8pm to 8am = CTime of 0-12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NUM_PIXELS, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
BlynkTimer timer;

//names widget rtc for real time clock
WidgetRTC rtc;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
/*

  https://www.youtube.com/watch?v=RAgLE-hV5cU

*/
char auth[] = "1234567890abcdefg"; //Blynk Project Auth Code. PUT YOUR OWN Auth Code HERE

//song stuff
int speed = 90; //higher value, slower notes
// notes in the song 'Mukkathe Penne'
int melody[] = {
  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4, 0,

  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,      //29               //8
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4, 0,

  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
  NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
  NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,             //58

  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
  NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,

  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, 0,
  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, END

};

// note durations: 8 = quarter note, 4 = 8th note, etc.
int noteDurations[] = {       //duration of the notes
  8, 4, 8, 4,
  4, 4, 4, 12,
  4, 4, 4, 4, 4, 4,
  4, 16, 4,

  8, 4, 8, 4,
  4, 2, 1, 1, 2, 1, 1, 12,
  4, 4, 4, 4, 4, 4,
  4, 16, 4,

  4, 4, 4, 4, 4, 4,
  4, 4, 4, 12,
  4, 4, 4, 4, 4, 4,
  4, 4, 4, 12,

  4, 4, 4, 4, 4, 4,
  2, 1, 1, 2, 1, 1, 4, 8, 4,
  2, 6, 4, 2, 6, 4,
  2, 1, 1, 16, 4,

  4, 8, 4, 4, 4,
  4, 16, 4,
  4, 8, 4, 4, 4,
  4, 20,
};
// Your WiFi credentials.
// Set password to "" for open networks.

  char ssid[] = "SSID";
  char pass[] = "PASS";


BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}
void setup()
{

  strip.begin();
  strip.clear();
  strip.show(); // Initialize all pixels to 'off'
  // Debug console
  Serial.begin(115200);

  
  pinMode(snooze, INPUT_PULLUP);
  
  for (int i = 12; i <= 28; i++) {
    int j = constrain(i-12, 0,11);//constrain hour test to 0-11 for 12 pixels
    strip.setPixelColor(i, 100, 100, 100);
    strip.setPixelColor(j,100,100,100);
    strip.show();
    delay(300);
    strip.clear();
    
  }
  strip.setPixelColor(0, 255, 0, 0);
  strip.setPixelColor(12, 255, 0, 0);
  strip.setPixelColor(20, 0, 255, 0);
  strip.show();
  Alarm();
  delay(500);
  strip.clear();
  strip.show();
setSyncInterval(1);
Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) { } //empty while loop to hold code while connection is established to server
  delay(1000);
clockDisplay();
  setSyncInterval(900000);  //every 15min



  // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);
}

void loop()
{
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V5)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Got a value: ");
  Serial.println(param.asStr());
  Alarm();
}

void Alarm()
{
  //song stuff
  for (int thisNote = 0; melody[thisNote] != -1; thisNote++) {
    int snoozeVal = digitalRead(snooze);


    int noteDuration = speed * noteDurations[thisNote];
    tone(speaker , melody[thisNote], noteDuration * .95);
    Serial.println(melody[thisNote]);
    //snooze button Exits function if pressed
    if (snoozeVal == LOW) {
      Serial.println("SNOOZE");
      return;
    }
    delay(noteDuration);
    //snooze button Exits function if pressed
    if (snoozeVal == LOW) {
      Serial.println("SNOOZE");
      return;
    }
    noTone(speaker);
  }
}

// Digital clock display of the time
void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
  int HOUR = hour();
  int MINUTE = minute();
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
  TimeColor();
  drawHourHand(HOUR);
  drawMinuteHand(MINUTE);
  strip.show();
  strip.clear();


}

void TimeColor() {
  int Hour = hour();
  if (Hour >= 8 || Hour <= 20) {
    CTime = Hour - 8;
    R = map(CTime, 0, 12, 255, 0);
    B = map(CTime, 0, 12, 0, 255);
    G = map(CTime, 0,12,200,0);
    BRIGHTNESSTIME=BRIGHTNESS;
  }
  if (Hour > 20 || Hour < 8) {
    if (Hour > 20) {
      CTime = Hour - 20;
    }
    else if (Hour < 8) {
      CTime = Hour + 4;
    }
    Serial.print("CTIME ");
    Serial.println(CTime);
    R = map(CTime, 0, 12, 0, 100);
    B = map(CTime, 0, 12, 200, 100);
    G = 0;
 BRIGHTNESSTIME=BRIGHTNESS*.3;
  
  }
  String RedGreenBlue = String(R) + " " + String(G) + " " + String(B);
  Serial.print("RGB " );
  Serial.println(RedGreenBlue);
}
/**
        setPixelColor -- set a single pixel in the NeoPixel ring to a specific RGB value
*/
void setPixelColor(Adafruit_NeoPixel & strip, int index, byte red, byte green, byte blue, int brightness )
{
  strip.setPixelColor(index,
                      (((int)red) * brightness) >> 8,
                      (((int)green) * brightness) >> 8,
                      (((int)blue) * brightness) >> 8
                     );
}

void drawMinuteHand(int minute)
{
  int pix_index = 16 * minute / 60;       // we only have a 16-pixel ring
  pix_index = map(pix_index, 0, 16, 16, 0);//Reverse order of pixels since ring is numbered COUNTER clockwise. Comment out if ring is numbered clockwise
  pix_index = pix_index + MINUTE_PIXEL_OFFSET;     // because the 12-ring is first
  int remainder = 16 * minute % 60;
  Serial.print("Remainder minutes ");
  Serial.println(remainder);

  int fade = map(remainder, 0, 60, 0, BRIGHTNESSTIME/2); //uses the remainder amount to calculate next pixel brightness
  int fadeinverse = map(remainder, 0, 60, BRIGHTNESSTIME/2, 0); //uses the remainder amount to calculate minute pixel brightness in relation to next pixel
  setPixelColor(strip, pix_index, R, G, B, fadeinverse); //set minute pixel brightness
  Serial.print("Minute Pixel Brightness ");
  Serial.println(fadeinverse);

  //set nex pixel numter. -1 because ring is numbered counter clockwise. If minute pixel is first pixel then previous minute should be last pixel on ring. 12 is first 27 is last. 
  int nextPixel = pix_index - 1;
  if (pix_index == 12) {
    nextPixel = 27;
  }
  setPixelColor(strip, nextPixel, R, G, B, fade);
  Serial.print("Minute nextPixel Brightness ");
  Serial.println(fade);


  Serial.println(pix_index);


}


/**
        drawHourHand -- update the inner NeoPixel ring, with a pixel representing the hour
*/
void drawHourHand(int Hour)
{
  Hour %= 12;
  if (Hour == 0 ) {
    // special case for midnight and noon
    setPixelColor(strip, Hour, R, G, B, BRIGHTNESSTIME);
    return;
  }
  // for hours 1 through 11
  int Fade = BRIGHTNESSTIME;
  for (int i = Hour; i >= 1; i--) {
    setPixelColor(strip, i, R, G, B, Fade);
    Fade = Fade - (Fade * .5);

  }
  Serial.println(Hour);

}
