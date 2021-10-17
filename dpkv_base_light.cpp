
int poluperiod;

void setup()

{

    pinMode(2, OUTPUT);

    pinMode(A1, INPUT);
}

void loop()

{

    int val = 70; // - 200 оборотов

    poluperiod = map(val, 0, 1023, 2500, 100);

    for (int a = 1; a <= gear - 2; a++)

    {

        digitalWrite(2, HIGH);

        delayMicroseconds(poluperiod);

        digitalWrite(2, LOW);

        delayMicroseconds(poluperiod);
    }

    delayMicroseconds(poluperiod * 4);
}
