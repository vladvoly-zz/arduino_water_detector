#define res_pin 0       //пин для резистора/аналогового датчика
int led13 = 13;          //пин цифровой для светодиода - red - высокая сухость
int led12 = 12;          //пин цифровой для светодиода - yellow - средняя сухость
int led11 = 11;          //пин цифровой для светодиода - green - нормальная сухость
int led10 = 10;           //пин цифровой для светодиода - green - низкая сухость
int led9 = 9;           //пин для цифрового датчика - blue LED - обнаружена протечка воды
int but = 8;            //пин кнопки для выключения клапана
int flam = 7;           //пин цифровой для датчика - сюда приходит значение с датчика
int relay = 6;          //реле на полив почвы
int relay2 = 5;         //реле на магнитный клапан для остановки протечки.
int val_flam;           //значение которое передает датчик цифровой
int val_but;            //значение для хранения кнопки.
int val1;               //значение аналогового датчика
int valr;               //значение с порта на приём с bluetooth датчика
unsigned long time;     //переменная для времени, сюда сохраняется время с функции millis

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);           //скорость передачи данных
  pinMode(led10,OUTPUT);          //назначение на выход сигнала с Arduino
  pinMode(led11,OUTPUT);          //назначение на выход сигнала с Arduino
  pinMode(led12,OUTPUT);          //назначение на выход сигнала с Arduino
  pinMode(led13,OUTPUT);          //назначение на выход сигнала с Arduino
  pinMode(led9,OUTPUT);           //назначение на выход сигнала с Arduino
  pinMode(relay,OUTPUT);          //назначение на выход сигнала с Arduino
  pinMode(relay2,OUTPUT);         //назначение на выход сигнала с Arduino
  pinMode(but,INPUT);         //назначение на вход сигнала в Arduino
  pinMode(flam,INPUT);        //назначение на вход сигнала в Arduino
  digitalWrite(led9,LOW);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(led10,LOW);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(led11,LOW);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(led12,LOW);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(led13,LOW);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(relay,HIGH);        //сигнал при старте Arduino имеет низкий уровень
  digitalWrite(relay2,HIGH);
}

void loop() 
{
  // put your main code here, to run repeatedly:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    val1 = analogRead(res_pin);             //считываем значение датчика влажности почвы
    val1 = map(val1,0,1023,0,100);          //конвертируем полученное значение в удобную нам систему, в данном случае в проценты
    val1 = constrain(val1,0,100);           //задаём повторно границы числа
                                            //приступаем к проверке датчика влажности почвы
    if(val1<20)                             //первый рубеж - до 20%
    {
      digitalWrite(led10, HIGH);
    }
    if(val1>20)                             //второй рубеж - после 20%
    {
      digitalWrite(led10, HIGH);
      digitalWrite(led11, HIGH);
    }
    else
    {
      digitalWrite(led11, LOW);
    }
    if(val1>60)                             //третий рубеж - после 60%
    {
      digitalWrite(led10, HIGH);
      digitalWrite(led11, HIGH);
      digitalWrite(led12, HIGH);; 
    }
    else
    {
      digitalWrite(led12, LOW);
    }
    if(val1>90)                             //четвертый рубеж - критический - после 90%
    {
      digitalWrite(led10, HIGH);
      digitalWrite(led11, HIGH);
      digitalWrite(led12, HIGH);
      digitalWrite(led13, HIGH);
      digitalWrite(relay,LOW);
      
      if((millis() - time) > 1000) {        //отправление сообщения раз в 1 сек. или(1000мкс.)
      time = millis();
      Serial.print("The soil is dry by ");
      Serial.print(val1);
      Serial.println(" %");
      }
    }
    else
    {
      digitalWrite(led13, LOW);
      digitalWrite(relay,HIGH);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                    //обработка значений детектора протечки воды
    val_flam = digitalRead(flam);                   //считываем значение детектора протечки воды
    if(val_flam==0)                                 //запускаем проверку состояния детектора - состояние обнаружения воды
    {
      digitalWrite(relay2,LOW);
      digitalWrite(led9,1);
      if((millis() - time) > 1000) {                //отправление сообщения раз в 1 сек. или(1000мкс.)
      time = millis();
      Serial.println("Water"); 
      }
    }
    val_but = digitalRead(but);
    if((millis() - time) > 5000) {                //задержка на выключение клапана и светодиода 
      time = millis();
      if(val_but==0)    //вторая проверка - если датчик просох, то система не откроет кран, так как нужно чтобы человек пришел и сам отключил систему.
      {
        digitalWrite(led9,0);
        digitalWrite(relay2,HIGH);
      }
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                    //обработка сигналов с Bluetooth модуля, или COM Port
    if(Serial.available()>0)
    {
      valr=Serial.read();
      if(valr=='d')                                 //если отправили букву 'd' - то получим значение с цифрового датчика(детектора протечки воды) 
      {
        Serial.println("No water - It`s OK");
      }
      if(valr=='a')                                 //если отправили букву 'a' - то получим значение с аналогового датчика(датчик влажности почвы)
      {
        Serial.print("The soil is dry by ");
        Serial.print(val1);
        Serial.println(" %"); 
      }
      if(valr=='c')                                 //если отправили букву 'c' - то сможем закрыть клапан подачи воды "вручную"  
      {
         digitalWrite(relay2,LOW);
         digitalWrite(led9,1);
      }
   }
}
