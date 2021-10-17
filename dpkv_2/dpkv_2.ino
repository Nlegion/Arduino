int poluperiod;
const int ledPin = 2;
long previousMillis = 0; // храним время последнего переключения светодиода
int ledState = LOW;      // этой переменной устанавливаем состояние светодиода
int gear = 10;
long interval = 1000;

void setup()

{
    pinMode(ledPin, OUTPUT);
    pinMode(A1, INPUT);
    //Serial.begin(9600);
}

void loop()

{
    unsigned long currentMillis = micros();
    //int val = 70; // - 200 оборотов
    //poluperiod = map(interval, 0, 1023, 2500, 100);

    for (int a = 1; a <= gear; a++)
    {
        //Serial.println(interval);
        //Serial.println(currentMillis);
        //Serial.println(previousMillis);
        if (currentMillis - previousMillis > interval)
        {
            previousMillis = currentMillis;
            if (a <= (gear-2))
            {

                if (ledState == LOW) {
                    ledState = HIGH;}
                else
                    {ledState = LOW;}
            }
            else
            {
                if (ledState == HIGH) {
                    ledState = LOW;}
            }
            digitalWrite(ledPin, ledState);
        }
    }
}
