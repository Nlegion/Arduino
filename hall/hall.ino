const int digital = 15; // Цифровой пин 2
const int analog = A0; // Аналоговый пин A0
 
void setup()
{
    pinMode(digital, INPUT);
    Serial.begin(9600);
}
 
void loop()
{
    Serial.print("Digital: ");
    Serial.print(digitalRead(digital)); // Цифровой сигнал с датчика
    Serial.print(", Analog: ");
    Serial.println(analogRead(analog)); // Аналоговый сигнал с датчика
    delay(500);
}
