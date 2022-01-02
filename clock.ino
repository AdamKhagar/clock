#include <iarduino_RTC.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define STOPWATCH_START_STOP_BUTTON_PIN 4
#define STOPWATCH_LED_PIN 13


iarduino_RTC time(RTC_DS1302,6,8,7);
LiquidCrystal_I2C LCD(0x27,16,2);


boolean stopwatchStartStopButtonLastState = false;
unsigned long int startTimeUnix;
String startTimeString;

void setup() {
  delay(300);
  LCD.init();
  LCD.backlight();
  time.begin();
  
  pinMode(STOPWATCH_START_STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOPWATCH_LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  //  Use this only first time to set time
  //  time.settime(0, 5, 19, 30, 12, 21, 4 ); 
}

void loop() {
  boolean stopwatchStartStopButton = !digitalRead(STOPWATCH_START_STOP_BUTTON_PIN);
  
  if (stopwatchStartStopButton) {
    // Stopwatch start-stop button click

    // double-check the state of the button to be sure it is pressed
    delay(10);
    stopwatchStartStopButton = !digitalRead(STOPWATCH_START_STOP_BUTTON_PIN);
    
    if (stopwatchStartStopButton) {
      if (!stopwatchStartStopButtonLastState) {
        // Stopwatch start
        stopwatchStartStopButtonLastState = true;
        startStopwatch();
      } else {
        // Stopwatch stop                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
        stopwatchStartStopButtonLastState = false;
        stopStopwatch();
      }
      delay(100);
    }
  } else if (millis() % 1000 == 0) {
    // Clock
    showClock();
    delay(1);
  }
}

void startStopwatch() {
  digitalWrite(STOPWATCH_LED_PIN, HIGH);
  showStopwatchStartMessage();
  startTimeUnix = time.gettimeUnix();
  startTimeString = time.gettime("H:i:s");
}


void showClock() {
  LCD.setCursor(0,0);
  LCD.print(time.gettime("d M Y, D"));
  LCD.setCursor(4,1);
  LCD.print(time.gettime("H:i:s"));
}


void showStopwatchStartMessage() {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Stopwatch: Start");

  for (int i = 1; i <= 5; i++) {
    LCD.setCursor(7, 1);
    LCD.print(i);
    delay(1000);
  }
  LCD.setCursor(7, 1);
  LCD.print("GO");
  delay(1000);
  LCD.clear();
}

String getTimeFromSeconds(unsigned long int seconds) {
  int s = seconds % 60;
  int m = seconds / 60;
  int h = m / 60;
  m = m % 60;

  if (h > 0) {
    return String(h) + "h " + m + "m " + s + "s";
  }
  else if (m > 0) {
    return String(m) + "m " + s + "s";
  }
  else {
    return String(s) + "s";
  }
}
  
 
void stopStopwatch() {
  unsigned long int seconds = time.gettimeUnix() - startTimeUnix;

  digitalWrite(STOPWATCH_LED_PIN, LOW);
  
  LCD.clear();
  // stopwatch-end screan
  LCD.setCursor(1, 0);
  LCD.print("Stopwatch: End");
  delay(2000);
  LCD.clear();

  // start-end time screan
  LCD.setCursor(0, 0);
  LCD.print("Start:  " + startTimeString);
  LCD.setCursor(0, 1);
  LCD.print("End:    " + String(time.gettime("H:i:s")));
  delay(4000);
  LCD.clear();

  //  total time screan
  String totalTime = getTimeFromSeconds(seconds);
  LCD.setCursor(0, 0);
  LCD.print("Total time:");
  LCD.setCursor(16 - totalTime.length(), 1);
  LCD.print(totalTime);
  delay(4000);
  LCD.clear();
}
