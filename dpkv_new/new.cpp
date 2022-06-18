const uint16_t rpm_D = 1000;        // по дефолту  1000 об/мин
const uint16_t tooth_quantity = 60; // выбираем количество зубчиков датчика коленвала
const uint16_t tooth_size = 3;      // выбираем длину зуба    в градусах
const uint16_t hole_size = 3;       // выбираем длину впадины в градусах
//    зуб+впадина = 360/количество зубов (проверяем это) !!!
const uint16_t missed_teeth = 2; // выбираем количество пропущенных зубьев
const bool CKPbeginLogic = 0;    // логика начала сигнала колена  (0 или 1)
#define CKPpin PB4               // пин коленвала
uint32_t T1, T2;                 // переменные времени прохождения шторки
uint32_t rpm_test = 0;           // количество оборотов коленвала по шторке
boolean running = false;         // маркер прохождения шторки
const int digital = 15;          // датчик холла

void setup()
{
    attachInterrupt(0, deteccion, RISING); //детекция
    pinMode(digital, INPUT);               // считывание датчика холла
    Serial.begin(115200);
    if ((tooth_size + hole_size) * tooth_quantity != 360)
        Serial.println("Неправильно выбрано количество и/или размер зубьев!!!");
    Serial.print(rpm_D);
    Serial.println(" rpm");
    Serial.println();
    DDRB |= _BV(CKPpin); // пин дпкв на выход
    if (!CKPbeginLogic)
        PORTB |= _BV(CKPpin); // задаем начальный уровень сигнала дпкв
    //ниже настраиваем аппаратный таймер ардуино
    TCCR1A = 0;
    TCCR1B = 8;
    TCCR1B |= 1;
    OCR1A = 2666666ul / rpm_D;
    TIMSK1 |= (1 << OCIE1A);
}

void loop() // луп в формированиим сигнала дпкв не участвует, здесь только задаются обороты коленвала через терминал
{
    if (running == true) // обновление оборотов двигателя
    {
        rpm_test = 60000000 / ((T2 - T1) * 2); // количество микросекунд в минуте / ((время полуоборота)*2)
        T1 = T2;
        running = false;
        Serial.println(rpm_test);
        OCR1A = 2666666UL / rpm_test;
    }
    if (Serial.available())
    {
        byte inbyte = Serial.read();
        if (inbyte >= '1' && inbyte <= '9') // в консоль вводим от 1 до 9, чтоб выбрать обороты(без символов конца строки)
        {
            uint16_t rpm = (inbyte - '0') * 1000;
            Serial.print(rpm);
            Serial.println(" rpm");
            OCR1A = 2666666UL / rpm;
        }
    }
}

void deteccion()
{
    if (digitalRead(digital) == 1)
    {
        T2 = micros();
    }

    running = true;
}

ISR(TIMER1_COMPA_vect) // обрабочик таймера, сюда заходит раз в градус поворота коленвала

{
    static byte countT = 0;                                                                 // счетчик градусов зуба/впадины
    static byte counttooth = 1;                                                             // счетчик зубов
    static bool tooth_or_hole = 1;                                                          // флаг зуб/впадина
    countT++;                                                                               // прибавляем счетчик градусов зуба/впадины
    if ((tooth_or_hole && countT >= tooth_size) || (!tooth_or_hole && countT >= hole_size)) // если по градусам достигли зуба или впадины
    {
        tooth_or_hole = !tooth_or_hole; // меняем состояние зуб/впадина
        countT = 0;                     // сбрасываем счетчик градусов зуба/впадины
        counttooth++;                   // увеличиваем номер текущего зуба
        if (counttooth > (tooth_quantity * 2))
            counttooth = 1; // сбрасываем увеличение номера зуба в случае достижения максимума
        // ниже дергаем ногой (сигнал ДПКВ), если это не пропущенные зубья.
        if ((tooth_quantity * 2 - counttooth) > (missed_teeth * 2 - 1))
            PORTB ^= _BV(CKPpin);
    }
}