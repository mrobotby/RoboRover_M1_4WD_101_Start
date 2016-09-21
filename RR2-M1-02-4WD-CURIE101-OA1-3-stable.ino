/*
    РОБОТЫ и НАБОРЫ ПО РОБОТОТЕХНИКЕ на МРобот! mrobot.by
    http://www.mrobot.by

    Кухня Роботов <maxxlife>
    http://www.vk.com/cookrobot
    Copyright (c) Кухня роботов <maxxlife.by> All rights reserved.
    Copyright (c) Макслайф Робот Maxxlife Robot All rights reserved.
    Наши лаборатории по робототехнике:
    Ленинская РОС ДОСААФ, ул. Рокоссовского 63/2, Минск, Беларусь
    Подробнее в нашей группе Вконтакте http://www.vk.com/cookrobot
    И на сайте http://www.maxxlife.by
    ****************************************************
    Мы всегда рады новым членам нашего сообщества Кухня Роботов.
    У нас есть вводные курсы, где мы объясняем
    как работать с нашими образовательными наборами по робототехнике и электронике.
    ****************************************************
    Название набора: РобоРовер М1 4WD
    Программа создана и протестирована разработчиком:
    Имя: Максим Массальский
    Ник: maxxlife
    E-mail: maxxliferobot@gmail.com
*/

/*В роботе используются пины (для Arduino Uno-подобной платы)
 * Мотордрайвер использует пины:
 * D2, D3, D5, D8, D10, D11
 * Сервопривод использует пины:
 * D9
 * Ультразвуковой датчик использует пины:
 * D4, D7
 * Свободные цифровые пины:
 * D0, D1, D6, D12, D13
 * Аналоговые ИК-датчики используют пины:
 * A0, A1
 * Свободные аналоговые пины:
 * A2, A3, A4, A5
 */

//Самодельный класс Servo, чтобы не использовать стандартную библиотеку Servo
//Т.к. она мешает управлению пинами мотордрайвера L298N
class Servo // имя класса
{
  public:
    int write(int data)
    {
      analogWrite(9, data);
    }
};
//Создаем экземпляр класса Servo под именем radar
Servo radar;

//Пины ультразвукового датчика
int trigPin = 7;
int echoPin = 4;

//Константа voltConst -это значение опорного напряжения в милливольтах
//Нужна для того, чтобы переводить значение напряжения с ИК-датчика в расстояние.
double voltConst = 0.0033;

//Определение номеров пинов мотордрайвера
int enA = 5;
int in1 = 2;
int in2 = 8;
// motor two
int enB = 3;
int in3 = 11;
int in4 = 10;

//Переменные по хранению расстояний с переднего левого и правого ИК-датчиков
volatile int rFront = 0;
volatile int lFront = 0;

// Поворотный сервопривод radar
volatile int posR;
//Значение для угла в 90 градусов (от 0 до 255)
int radarFront = 185;
//Значение для левого положения
int radarLeft = 140;
//Значение для правого положения
int radarRight = 230;

// Ультразвуковой сенсор
//Переменная по хранению расстояния с УЗ-датчика
volatile long usRange = 0;
//Максимальный порого
int usHigh = 100;
//Минимальный порог
int usLow = 30;
volatile long usRangeLeft = 0;
volatile long usRangeRight = 0;

// Датчики инфракрасные Передние
//double angleFront=cos((30*PI)/180);// Угол 30 градусов
float angleFront = 1;
//Левый ИК-датчик подключен к аналоговому пину 0
int irPinFrontLeft = 0;
//Правый ИК-датчик подключен к аналоговому пину 1
int irPinFrontRight = 1;

//Переменные по настройке алгоритма передних ИК-датчиков под условия и расстояния
//Максимальный порог
int frontHigh = 40;
//Минимальный порог
int frontLow = 50;

//Класс для измерения расстояния с ИК-датчиков робота
class irSharpMeasure
{
  public:
    int irMeasure(int irPin)
    {
      int distance;
      // Измеряем значение с датчика в милливольтах.
      //Значение, которое было считано с аналогового порта умножается на переменную voltConst
      //И получается значение в вольтах, которое есть на аналоговом пине
      double volts = analogRead(irPin) * voltConst;

      //Serial.println(volts);
      //Экспериментальные значения в вольтах для определенного расстояния
      //Определены экспериментальным путем и с использованием даташита к ИК-датчику Sharp (10-80cm)
    double val[40] = { 2.73, 2.34, 1.99, 1.76, 1.57, 1.42, 1.29, 1.20, 1.07, 1.00, 0.94, 0.88, 0.82, 0.79, 0.74, 0.71, 0.68, 0.65, 0.63, 0.61, 0.58, 0.55, 0.53, 0.50, 0.51, 0.49, 0.48, 0.47, 0.45, 0.44, 0.42, 0.41, 0.40, 0.39, 0.38, 0.37, 0.36, 0.33, 0.28, 0.26};   
      int cm[40] = {8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 90, 100, 110};
      //Алгоритм по сопоставлению значения в вольтах расстоянию
      for (int i = 0; i < 40; i++)
      {
        if (val[i] <= volts)
        {
          //Находим расстояние
          //Если датчик находится под углом, то можно узнать расстояние до объекта по теореме Пифагора, если это необходимо
          //Умножив на angleFront
          distance = cm[i] * angleFront;
          break;
        }
        //Если volts меньше определенного порога, то считаем, что расстояние больше 120 см
        if (volts < 0.26)
        {
          distance = 120 * angleFront;
        }
      }
      //Если volts больше определенного порогоа, то считаем, что расстояние меньше 10 см
      if (volts > 3.00)
      {
        distance = 6* angleFront;
      }
      //Возвращаем значение distance
      return distance;
    }
};
//Создаем экземпляр класса под именем IR
irSharpMeasure ir;

//Функция по измерению расстояний с переднего левого и правого ИК-датчиков
void irMeasure()
{
  //Меряем расстояние с правого ИК
  rFront = ir.irMeasure(irPinFrontRight);
  //Меряем расстояние с левого ИК
  lFront = ir.irMeasure(irPinFrontLeft);
}

//Функция по измерению расстояния с УЗ-датчика HCSR-04
void usMeasure () {

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  usRange = (duration / 2) / 29.1;
  delay(50);
}

//Функция по выезду из тупика при помощи ИК-датчиков и УЗ-датчика
void irFrontGoBack()
{
  //Если с двух передних ИК-датчиков расстояния одинаковые,
  //То используем ультразвуковой датчик
  if (lFront == rFront)
  {
    //Поворачиваем сначала УЗ-датчик вправо
    radar.write(radarRight);
    delay(200);
    //Делаем измерением
    usMeasure();
    //Записываем расстояние справа в отдельную переменную
    usRangeRight = usRange;
    //Поворачиваем влево
    radar.write(radarLeft);
    delay(200);
    //Делаем измерение
    usMeasure();
    //Записываем расстояние слева в отдельную переменную
    usRangeLeft = usRange;
    //Если расстояние слева меньше расстояния справа
    if (usRangeLeft < usRangeRight)
    {
      //То поворачиваем вправо
      go_RightFull(200,100);
      delay(500);
      go_Stop();
      //Поворачиваем УЗ влево
      radar.write(radarLeft);

      //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
      //Можешь настраивать как тебе необходимо
      while ((rFront < frontLow) && (lFront < frontLow))
      {
        irMeasure();
        go_Backward(220,220);
      }
      //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
      usMeasure();
      while ( usRange < usLow)
      {
        usMeasure();
        go_Backward(220,220);
      }
      //Поворачиваем робота вправо
      go_RightFull(200,100);
      delay(500);
      go_Stop();
    }
    //Здесь как и выше описано, только все операции наоборот.
    if (usRangeRight < usRangeLeft)
    {
      go_LeftFull(100,200);
      delay(500);
      go_Stop();
      radar.write(radarRight);
      while ((rFront < frontLow) && (lFront < frontLow))
      {
        irMeasure();
        go_Backward(220,220);
      }
      usMeasure();
      while (usRange < usLow)
      {
        usMeasure();
        go_Backward(220,220);
      }
      go_LeftFull(100,200);
      delay(500);
      go_Stop();
    }
    //Если у нас произошло, что и УЗ-датчика одинаковое расстояние(!), робот просто едет назад
    //До тех пор пока не будут удовлетворены условия
    if (usRangeRight == usRangeLeft)
    {
      radar.write(radarFront);
      while ((rFront < frontLow) && (lFront < frontLow))
      {
        irMeasure();
        go_Backward(220,220);
      }
      usMeasure();
      while ((usRange < usLow))
      {
        usMeasure();
        go_Backward(220,220);
      }
    }
    radar.write(radarFront);
  }
  //Если с правого ИК-датчика расстояние меньше чем с левого ИК-датчика
  if (rFront < lFront)
  {
    //Поворачиваем робота влево
    go_LeftFull(100,200);
    delay(500);
    go_Stop();
    //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
    //Можешь настраивать как тебе необходимо.
    while ((rFront < frontLow) && (lFront < frontLow))
    {
      irMeasure();
      go_Backward(220,220);
    }
    usMeasure();
    //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
    while ((usRange < usLow))
    {
      usMeasure();
      go_Backward(220,220);
    }
    //Поворачиваем робота влево
    go_LeftFull(100,200);
    delay(500);
    go_Stop();
  }
  //Если с левого ИК-датчика расстояние меньше чем с правого ИК-датчика
  if (lFront < rFront)
  {
    //Поворачиваем робота вправо
    go_RightFull(200,100);
    delay(500);
    go_Stop();
    //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
    //Можешь настраивать как тебе необходимо.
    while ((rFront < frontLow) && (lFront < frontLow))
    {
      go_Backward(220,220);
      irMeasure();
    }
    usMeasure();
    //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
    while ((usRange < usLow))
    {
     usMeasure();
      go_Backward(220,220);
    }
    //Поворачиваем робота вправо
    go_RightFull(200,100);
    //Задержка обуславливает длительность поворота
    delay(500);
    //Остановка
    go_Stop();
  }
}
//Функция по измерению расстояния с УЗ-датчика
//В трех пололожениях. По центру, крайнее правое и крайнее левое.
void usRadar()
{
  radar.write(radarRight);
  delay(250);
  usMeasure();
  usRangeRight = usRange;
  radar.write(radarLeft);
  delay(250);
  usMeasure();
  usRangeLeft = usRange;
}

//Функция для мотордрайвера по движению робота прямо
//A-левый
//B-правый
int go_Forward(int a,int b)
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  //Настройка скорости вращения мотора, используя ШИМ (от 0 до 255)
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  //Настройка скорости вращения мотора, используя ШИМ (от 0 до 255)
  analogWrite(enB, b);
}

//Функция, которая поворачивает влево
//Правые колеса едут на вперед
//Левые колеса едут на назад
int go_LeftFull(int a,int b)
{
  //Левый мотор
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, b);
}
// Функция легкого поворота влево.
//Просто замедляется скорость вращения левых колес
void go_LeftHalf(int a, int b)
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, b);
}

//Функция, которая поворачивает вправо
//Левые колеса едут вперед
//Правые колеса едут назад
void go_RightFull(int a,int b)
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, b);
}
// Функция легкого поворота вправо
//Просто замедляется скорость вращения правых колес
void go_RightHalf(int a,int b)
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, b);
}
//Функция по движению назад
int go_Backward(int a, int b)
{
  //Левый мотор
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, a);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, b);
}
void go_Stop()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 200);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 200);
}

// Функция setup() выполняется каждый раз, когда будет перезапущена подача питания
// или будет произведена перезагрузка платы
void setup() {
  //Можно инициализровать Serial чтобы производить отладку.
  //Serial.begin(9600);
  //Установка сервопривода в центральное положение
  radar.write(radarFront);
  delay(5);
  //Инициализация пинов УЗ-датчика на входи и на выход
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(5);
  //Инициализация пинов мотордрайвера на выход
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Для отладки можно еще настроить встроенный светодиод на 13 пине
  //pinMode(13,OUTPUT);
  delay(1000);
}

//Главный бесконечный цикл, в котором находится управляющие операторы
//В роботе четыре колеса без резиновых колец
void loop() {

  //Вызов функции по измерению расстояний с ИК-датчиков
  //Необходимо всегда ее вызывать, чтобы обновлялись значения
  irMeasure();
  //Вызов функции по измерению расстояний с УЗ-датчика
  //Необходимо всегда ее вызывать, чтобы обновлялись значения
  usMeasure();
  //Принятие решения на основе показаний с ИК-датчиков
  //Если с левого и правого ИК-датчика расстояние больше определенного значения
  //То робот едет прямо
  if ((lFront > 50) && (rFront > 50))
  {
    //  digitalWrite(13,HIGH);
    go_Forward(220,220);
  }
  //В другом случае
  else
  {
    //Если с левого и правого ИК-датчика расстояние маленькое и с УЗ-датчика маленькое
    if ((lFront <= 30) && (rFront <= 30) || (usRange <= 15))
    {
      //То думаем, что робот рядом со стеной
      //Вызываем функцию по движению робота назад и выезду из тупика
      irFrontGoBack();
    }
    //В другом случае
    else
    {
      //Если расстояние с правого ИК-датчика меньше чем с левого ИК-датчика
      if ((rFront <= lFront))
      {
        //То робот поворачивает влево
        go_LeftFull(100,180);
      }
      //Если расстояние с левого ИК-датчика меньше чем с правого ИК-датчика
      if (lFront < rFront)
      {
        //То робот поворачивает вправо
        go_RightFull(180,100);
      }
    }
  }
}

