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
}

void loop()

{
    unsigned long currentMillis = micros();

    for (int a = 1; a <= gear; a++)
    {

        if (currentMillis - previousMillis > interval)
        {
            previousMillis = currentMillis;
            if (a <= (gear - 2))
            {

                if (ledState == LOW)
                {
                    ledState = HIGH;
                }
                else
                {
                    ledState = LOW;
                }
            }
            else
            {
                if (ledState == HIGH)
                {
                    ledState = LOW;
                }
            }
            digitalWrite(ledPin, ledState);
        }
    }
}