int gear = 60;  // тип  шкива коленвала
int poluperiod; // задержка

void setup()
{
    pinMode(2, OUTPUT);
    pinMode(A1, INPUT); // вход потенциометра
}

void loop()
{
    //  int val = analogRead(A1); // значения от 0 - 1023
    int val = 70;

    if (gear == 60)
    {
        poluperiod = map(val, 0, 1023, 2500, 100); // переприсвоение диапазона значений от 2500 мкс до 100 мкс
    }
    else if (gear == 36)
    {
        poluperiod = map(val, 0, 1023, 4200, 168);
    }
    for (int a = 1; a <= gear - 2; a++) // количество импульсов - 2 зуба
    {
        digitalWrite(2, HIGH);
        delayMicroseconds(poluperiod);
        digitalWrite(2, LOW);
        delayMicroseconds(poluperiod);
    }
    delayMicroseconds(poluperiod * 4); // количество импульсов до задержки в 2 зуба
}
