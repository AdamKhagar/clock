#include <iarduino_RTC.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//#define STOPWATCH_START_STOP_BUTTON_PIN 4
//#define STOPWATCH_LED_PIN 13


enum {ON, PAUSE, OFF} STATE;

iarduino_RTC time(RTC_DS1302,6,8,7); //  DS1302 - RST, CLK, DAT nm                                                                                                                                                                                                                                                                                 
LiquidCrystal_I2C LCD(0x27,16,2);


// Stopwatch: Start
class Stopwatch {
  public:
    Stopwatch(int startBtnPin, int pauseBtnPin, int showBtnPin, int ledPin = 13);
    
    int state();
    void init();
    bool isActive();
    void run();
    
  private:
    // pins {
    int _ledPin;
    int _startBtnPin;
    int _pauseBtnPin;
    int _showBtnPin;
    // }
    int _state = OFF;
    bool _startBtnState;
    bool _pauseBtnState;
    bool _showBtnState;
    unsigned long int _startTimeUnix;
    unsigned long int _lastTimeUnix;
    unsigned long int _activeTime = 0;
    String _startTimeString;
        
    void _readButtons();
    void _start();
    void _showStartMessage();
    void _pause();
    void _showPauseMessage(bool clear = true);
    void _continue();
    void _showContinueMessage();
    void _stop();
    void _showStopMessage();
    String _getTotalTime(unsigned long int seconds);
    void _showStatus();
};


Stopwatch::Stopwatch(int startBtnPin, int pauseBtnPin, int showBtnPin, int ledPin) {
  _ledPin = ledPin;
  _startBtnPin = startBtnPin;
  _pauseBtnPin = pauseBtnPin;
  _showBtnPin = showBtnPin;
}


void Stopwatch::init() {
  pinMode(_ledPin, OUTPUT);
  pinMode(_startBtnPin, INPUT_PULLUP);
  pinMode(_pauseBtnPin, INPUT_PULLUP);
  pinMode(_showBtnPin, INPUT_PULLUP);
}


void Stopwatch::_showStartMessage() {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Stopwatch: Start");

  for (int i = 1; i <= 3; i++) {
    LCD.setCursor(7, 1);
    LCD.print(i);
    delay(500);
  }
  LCD.setCursor(7, 1);
  LCD.print("GO");
  delay(500);
  LCD.clear();
}


void Stopwatch::_start() {
  digitalWrite(_ledPin, HIGH);
  _showStartMessage();
  _state = ON;
  _startTimeUnix = time.gettimeUnix();
  _lastTimeUnix = time.gettimeUnix();
  _startTimeString = time.gettime("H:i:s");
}


String Stopwatch::_getTotalTime(unsigned long int seconds) {
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


void Stopwatch::_showStopMessage() {
  String totalTime = _getTotalTime(time.gettimeUnix() - _startTimeUnix);
  String activeTime = _getTotalTime((time.gettimeUnix() - _lastTimeUnix) + _activeTime);
  
  LCD.clear();
  // stopwatch-end screan
  LCD.setCursor(1, 0);
  LCD.print("Stopwatch: End");
  delay(2000);
  LCD.clear();

  // start-end time screan
  LCD.setCursor(0, 0);
  LCD.print("Start:  " + _startTimeString);
  LCD.setCursor(0, 1);
  LCD.print("End:    " + String(time.gettime("H:i:s")));
  delay(5000);
  LCD.clear();

  //  total time screan
  LCD.setCursor(0, 0);
  LCD.print("Total time:");
  LCD.setCursor(16 - totalTime.length(), 1);
  LCD.print(totalTime);
  delay(5000);
  LCD.clear();

  //  active time screan
  LCD.setCursor(0, 0);
  LCD.print("Active time:");
  LCD.setCursor(16 - activeTime.length(), 1);
  LCD.print(activeTime);
  delay(5000);
  LCD.clear();
}


void Stopwatch::_stop() {
  _state = OFF;
  _showStopMessage( );
  digitalWrite(_ledPin, LOW);
}


void Stopwatch::_readButtons() {
  _startBtnState = !digitalRead(_startBtnPin);
  _pauseBtnState = !digitalRead(_pauseBtnPin);
  _showBtnState = !digitalRead(_showBtnPin);
}


void Stopwatch::_pause() {
  _state = PAUSE;
  _activeTime += time.gettimeUnix() - _lastTimeUnix;
  _showPauseMessage();
}


void Stopwatch::_showPauseMessage(bool clear = true) {
  if (clear) {
    LCD.clear();
  }
  LCD.setCursor(0, 0);
  LCD.print("Stopwatch: Pause");
  delay(500);
}


void Stopwatch::_continue() {
  _state = ON;
  _lastTimeUnix = time.gettimeUnix();
  _showContinueMessage();
}


void Stopwatch::_showContinueMessage() {
  LCD.clear();
  LCD.setCursor(3, 0);
  LCD.print("Stopwatch:");
  for (int i = 1; i <= 3; i++) {
    LCD.setCursor(7, 1);
    LCD.print(i);
    delay(500);
  }
  LCD.setCursor(4, 1);
  LCD.print("Continue");
  delay(1000);
}


void Stopwatch::_showStatus() {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Stopwatch:Status");
  LCD.setCursor(0, 1);
  LCD.print("Start:  " + _startTimeString);
  delay(5000);


  String totalTime = _getTotalTime(time.gettimeUnix() - _startTimeUnix);
  String activeTime = _getTotalTime((time.gettimeUnix() - _lastTimeUnix) + _activeTime);
  
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Total time:");
  LCD.setCursor(16 - totalTime.length(), 1);
  LCD.print(totalTime);
  delay(5000);

  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Active time:");
  LCD.setCursor(16 - activeTime.length(), 1);
  LCD.print(activeTime);
  delay(5000);
  LCD.clear();
}


bool Stopwatch::isActive() {
  _readButtons();
  if (_startBtnState || _pauseBtnState || _showBtnState){
    delay(10);
    _readButtons();
    return (_startBtnState || _pauseBtnState || _showBtnState);
  } else if (_state == PAUSE) {
    return true;
  } else {
    return false;
  }
}


void Stopwatch::run() {
  if (_startBtnState) {
    switch (_state) {
      case OFF:
        _start();
        break;
      case ON:
        _stop();
        break;
      case PAUSE:
        _continue();
        break;
    }
  } else if (_pauseBtnState && _state != OFF) {
    switch (_state) {
      case ON:
        _pause();
        break;
      case PAUSE:
        _continue();
        break;
    }
  } else if (_showBtnState && _state != OFF) {
    _showStatus();
  } else if (_state == PAUSE) {
    _showPauseMessage(false);
  }
}
// Stopwatch: end

Stopwatch stopwatch(4, 3, 5, 13);

void setup() {
//  delay(300);
  LCD.init();
  LCD.backlight();
  time.begin();
  stopwatch.init(); 
  //  Use this only first time to set time
  //  time.settime(0, 5, 19, 30, 12, 21, 4 ); 
}


void loop() {
  if (stopwatch.isActive()) {
    stopwatch.run();
  } else if (millis() % 1000 == 0) {
    // Clock
    showClock();
    delay(1);
  }
}

void showClock() {
  LCD.setCursor(0,0);
  LCD.print(time.gettime("d M Y, D"));
  LCD.setCursor(4,1);
  LCD.print(time.gettime("H:i:s"));
}
