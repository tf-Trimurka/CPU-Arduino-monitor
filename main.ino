#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define pot A0
// Инициализация LCD (адрес может быть 0x27 или 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Адрес 0x27, 16 столбцов, 2 строки

// Пины для светодиодов
const int ledPins[] = {3, 9, 5, 6, 10};
const int numLeds = 5;

// Переменные для хранения данных
int cpuUsage = 0;
int ramUsage = 0;
byte LedLev = 200;
// Буфер для приема данных
String inputString = "";
bool stringComplete = false;

void setup() {
  Serial.begin(9600);
  // Настройка пинов светодиодов
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }
  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
  
   inputString.reserve(100); // Резервируем память для строки
   while(!Serial.available()){
    potento();
    for(int i = 0; i < numLeds; i++){
      analogWrite(ledPins[i],LedLev);
    }
   }
}

void loop() {
  // Обработка входящих данных
  if (stringComplete) {
    parseData(inputString);
    updateDisplay();
    updateLEDs();
    inputString = "";
    stringComplete = false;
  }
}
void potento(){
  LedLev = map(analogRead(pot),0,1023,0,255);
}

// Функция обработки последовательного порта
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

// Парсинг данных в формате "CPU:45%,RAM:67%"
void parseData(String data) {
  // Ищем позиции меток
  int cpuPos = data.indexOf("CPU:");
  int ramPos = data.indexOf("RAM:");
  
  if (cpuPos != -1 && ramPos != -1) {
    // Извлекаем значения
    cpuUsage = data.substring(cpuPos + 4, data.indexOf('%', cpuPos)).toInt();
    ramUsage = data.substring(ramPos + 4, data.indexOf('%', ramPos)).toInt();
    
    // Ограничиваем значения 0-100%
    cpuUsage = constrain(cpuUsage, 0, 100);
    ramUsage = constrain(ramUsage, 0, 100);
  }
}

// Обновление дисплея
void updateDisplay() {
  lcd.clear();
  
  // Первая строка: CPU
  lcd.setCursor(0, 0);
  lcd.print("CPU: ");
  lcd.print(cpuUsage);
  lcd.print("%");
  
  // Вторая строка: RAM
  lcd.setCursor(0, 1);
  lcd.print("RAM: ");
  lcd.print(ramUsage);
  lcd.print("%");
}

// Управление светодиодами в зависимости от загрузки CPU
// Альтернативная версия - прогрессивное включение
void updateLEDs() {
  int ledsToLight = map(cpuUsage, 0, 100, 0, numLeds+1);
  
  for (int i = 0; i < numLeds; i++) {
    analogWrite(ledPins[i], (i < ledsToLight) ? LedLev : 0);
  }
}
