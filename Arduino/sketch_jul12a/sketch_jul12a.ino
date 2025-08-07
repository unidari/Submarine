#include <Servo.h>

#include <NewPing.h>

#define TRIGGER_PIN  10  // Пин для триггера
#define ECHO_PIN     12  // Пин для эхо
#define TRIGGER_PIN1 7  // Пин для триггера
#define ECHO_PIN1    5  // Пин для эхо
#define MAX_DISTANCE 500 // Максимальная длина

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing подстраивает пины и максимальную длину (так максимальна дистанция - 400-500 см).
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE); // NewPing подстраивает пины и максимальную длину (так максимальна дистанция - 400-500 см).

Servo servo;


int lambda = 4; // Для усреднения значения измерения (считаем по данным датчика - погрешность 3 см)

void setup() {
  servo.attach(9); // Пин для сервопривода
  Serial.begin(9600); // Подключаем монитор порта
  while (!Serial); // Ожидание подключения к последовательному порту
  delay(2000);
  servo.write(90); // Инициализация сервопривода в нейтральное положение - 0 градусов
}
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "START" && !hasScanned) {
      shouldScan = true;
      hasScanned = true; // Устанавливаем флаг, что сканирование начато
    } else if (command == "STOP") {
      shouldScan = false;
      hasScanned = false; // Сбрасываем флаг при остановке
    } else if (command == "RESET") {
      hasScanned = false; // Команда для сброса состояния
    }
  }

  if (shouldScan) {
    performScan();
    shouldScan = false; // После завершения сканирования сбрасываем флаг
  }
}
void performScan() {
  int i = 0; // Для угла поворота
  int t = 0; // Для усреднения значения (цикл)
  int a = 0; // Переменная, в которой будет храниться усреднённое значение
  int a1 = 0; // Переменная, в которой будет храниться усреднённое значение - 1
  
  for (i=0; i < 180 && shouldScan; i++) {
    
    servo.write(i); // Поворот на i градус c
    delay(40); // Пауза для стабилизации сервопривода

    for (t = 0; t < lambda; t++)
    {
      if (!shouldScan) break;
      signal = sonar.ping();
      signal1 = sonar1.ping();
      a = signal/US_ROUNDTRIP_CM + a; // усреднение значения
      a1 = signal1/US_ROUNDTRIP_CM + a1; // усреднение значения
      delay(60); // пауза
    }
    if (!shouldScan) break;
    a = a / (lambda-1); // само среднее значение
    a1 = a1 / (lambda-1); // само среднее значение
    t = 0;
    Serial.print(i);     // Выводим угол для певрой переменной
    Serial.print(",");   // Выводим запятую
    Serial.println(a);   // Выводим первую переменную
    Serial.print(i+180);     // Выводим угол для певрой переменной
    Serial.print(",");   // Выводим запятую
    Serial.println(a1);   // Выводим первую переменную
    a = 0;
    a1 = 0;
    // выводим среднее значение и сбрасываем переменные для следующего выволнения
    // по итогу, на каждый вызов мы затратим 50 микросекунд
    // 20 мкс - первый вызов, 10 мкс - наименьшее время для подачи сигнала и его приёма -- для одного датчика
    // * 40 и 60 - параметры для двух датчиков
  }
  
}