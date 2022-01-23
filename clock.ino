#include <iarduino_RTC.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// RGB LED
#define RED 9
#define GREEN 10
#define BLUE 11

// BUTTONS
#define BUTTON_1 2
#define BUTTON_2 A2
#define BUTTON_3 12
#define BUTTON_4 A3

// DS1302 
#define RST 6
#define CLK 7
#define DAT 8

// Delay time 
#define SCREEN_LONG_DELAY 2000
#define SCREEN_SHORT_DELAY 1000

enum {ON, PAUSE, OFF} STATE;

iarduino_RTC time(RTC_DS1302, RST, CLK, DAT); //  DS1302 - RST, CLK, DAT nm                                                                                                                                                                                                                                                                                 
LiquidCrystal_I2C LCD(0x27,16,2);


class RGB_LED {
  public: 
    RGB_LED(uint8_t r, uint8_t g, uint8_t b);

    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void red();
    void green();
    void blue();
    void yellow();
    void off();
    void init();

  private:
    uint8_t _rPin;
    uint8_t _gPin;
    uint8_t _bPin;
  
};

RGB_LED::RGB_LED(uint8_t r, uint8_t g, uint8_t b) {
  _rPin = r;
  _gPin = g;
  _bPin = b;  
}

void RGB_LED::init() {
  pinMode(_rPin, OUTPUT);
  pinMode(_gPin, OUTPUT);
  pinMode(_bPin, OUTPUT);  
}

void RGB_LED::setColor(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(_rPin, r);
  analogWrite(_gPin, g);
  analogWrite(_bPin, b);
}

void RGB_LED::red() {
  analogWrite(_rPin, 10);
  analogWrite(_gPin, 0);
  analogWrite(_bPin, 0);
}

void RGB_LED::green() {
  analogWrite(_rPin, 0);
  analogWrite(_gPin, 30);
  analogWrite(_bPin, 0);
}

void RGB_LED::blue() {
  analogWrite(_rPin, 0);
  analogWrite(_gPin, 0);
  analogWrite(_bPin, 30);
}

void RGB_LED::yellow() {
  analogWrite(_rPin, 30);
  analogWrite(_gPin, 30);
  analogWrite(_bPin, 0);
}

void RGB_LED::off() {
  analogWrite(_rPin, 0);
  analogWrite(_gPin, 0);
  analogWrite(_bPin, 0);
}


class Stopwatch {
  public:
    Stopwatch(uint8_t startBtnPin, uint8_t pauseBtnPin, uint8_t showBtnPin);
    uint8_t state();
    void init();
    bool isActive();
    void run();
    const RGB_LED *rgb;
    
  private:
    uint8_t _startBtnPin;
    uint8_t _pauseBtnPin;
    uint8_t _showBtnPin;
    uint8_t _state = OFF;
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
    void _showPauseMessage();
    void _continue();
    void _showContinueMessage();
    void _stop();
    void _showStopMessage();
    String _getTime(unsigned long int seconds);
    String _getTotalTime(unsigned long now = time.gettimeUnix());
    String _getActiveTime(unsigned long now = time.gettimeUnix());
    void _showStatus();
    void _printActiveTime(String _time);
    void _printTotalTime(String _time);
};

Stopwatch::Stopwatch(uint8_t startBtnPin, uint8_t pauseBtnPin, uint8_t showBtnPin) {
  _startBtnPin = startBtnPin;
  _pauseBtnPin = pauseBtnPin;
  _showBtnPin = showBtnPin;
  rgb = new RGB_LED(RED, GREEN, BLUE);
}

void Stopwatch::init() {
  pinMode(_startBtnPin, INPUT_PULLUP);
  pinMode(_pauseBtnPin, INPUT_PULLUP);
  pinMode(_showBtnPin, INPUT_PULLUP);
  rgb->init();
}

void Stopwatch::_showStartMessage() {
  LCD.clear();
  LCD.setCursor(5, 0);
  LCD.print("Start");;
  delay(SCREEN_SHORT_DELAY);
  LCD.clear();
}

void Stopwatch::_start() {
  rgb->blue();
  _showStartMessage();
  _state = ON;
  _startTimeUnix = time.gettimeUnix();
  _lastTimeUnix = _startTimeUnix;
  _startTimeString = time.gettime("H:i:s");
}

String Stopwatch::_getTime(unsigned long int seconds) {
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

String Stopwatch::_getTotalTime(unsigned long now = time.gettimeUnix()) {
  return _getTime(now - _startTimeUnix);
}

String Stopwatch::_getActiveTime(unsigned long now = time.gettimeUnix()) {
  return _getTime((now - _lastTimeUnix) + _activeTime);
}

void Stopwatch::_printActiveTime(String _time) {
  LCD.setCursor(0, 0);
  LCD.print("Active time:");
  LCD.setCursor(16 - _time.length(), 1);
  LCD.print(_time);
  delay(SCREEN_LONG_DELAY);
  LCD.clear();
}

void Stopwatch::_printTotalTime(String _time) {
  LCD.setCursor(0, 0);
  LCD.print("Total time:");
  LCD.setCursor(16 - _time.length(), 1);
  LCD.print(_time);
  delay(SCREEN_LONG_DELAY);
  LCD.clear();
}

void Stopwatch::_showStopMessage() {
  unsigned long int now = time.gettimeUnix();
  String totalTime = _getTotalTime(now);
  String activeTime = _getActiveTime(now);
  
  LCD.clear();
  // stopwatch-end screan
  LCD.setCursor(6, 0);
  LCD.print("End");
  delay(SCREEN_SHORT_DELAY);
  LCD.clear();

  // start-end time screan
  LCD.setCursor(0, 0);
  LCD.print("Start:  " + _startTimeString);
  LCD.setCursor(0, 1);
  LCD.print("End:    " + String(time.gettime("H:i:s")));
  delay(SCREEN_LONG_DELAY);
  LCD.clear();

  _printTotalTime(totalTime);
  _printActiveTime(activeTime);
}

void Stopwatch::_stop() {
  _state = OFF;
  _showStopMessage( );
  rgb->off();
}

void Stopwatch::_readButtons() {
  _startBtnState = !digitalRead(_startBtnPin);
  _pauseBtnState = !digitalRead(_pauseBtnPin);
  _showBtnState = !digitalRead(_showBtnPin);
}

void Stopwatch::_pause() {
  _state = PAUSE;
  rgb->yellow();
  _activeTime += time.gettimeUnix() - _lastTimeUnix;
  _showPauseMessage();
}

void Stopwatch::_showPauseMessage() {
  LCD.clear();
  LCD.setCursor(5, 0);
  LCD.print("Pause");
  delay(SCREEN_SHORT_DELAY);
}

void Stopwatch::_continue() {
  _state = ON;
  rgb->blue();
  _lastTimeUnix = time.gettimeUnix();
  _showContinueMessage();
}

void Stopwatch::_showContinueMessage() {
  LCD.clear();
  LCD.setCursor(4, 0);
  LCD.print("Continue");
  delay(SCREEN_SHORT_DELAY);
}

void Stopwatch::_showStatus() {
  LCD.clear();
  LCD.setCursor(5, 0);
  LCD.print("Status");
  delay(SCREEN_SHORT_DELAY);

  unsigned long now = time.gettimeUnix();
  String totalTime = _getTotalTime(now);
  String activeTime = _getActiveTime(now);
  
  _printTotalTime(totalTime);
  _printActiveTime(activeTime);
}

bool Stopwatch::isActive() {
  _readButtons();
  if (_startBtnState || _pauseBtnState || _showBtnState){
    delay(10);
    _readButtons();
    return (_startBtnState || _pauseBtnState || _showBtnState);
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
    _showPauseMessage();
  }
}


Stopwatch stopwatch(BUTTON_3, BUTTON_4, BUTTON_2);

void setup() {
  LCD.init();
  LCD.backlight();
  time.begin();
  stopwatch.init(); 
  //  Use this only first time to set time, then after that upload code again but this time with commented string
  //  time.settime(0, 46, 22, 22, 1, 22, 6); // seconds, minuts, hours, day, month, year, weekday
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
  LCD.setCursor(4,1);
  LCD.print(time.gettime("H:i:s"));
  LCD.setCursor(0,0);
  LCD.print(time.gettime("d M Y, D"));
}
