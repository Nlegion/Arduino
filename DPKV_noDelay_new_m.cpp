const uint16_t rpm_D = 1000;        // по дефолту  1000 об/мин
const uint16_t tooth_quantity = 60; // выбираем количество зубчиков датчика коленвала
const uint16_t tooth_size = 3;      // выбираем длину зуба    в градусах
const uint16_t hole_size = 3;       // выбираем длину впадины в градусах
//    зуб+впадина = 360/количество зубов (проверяем это) !!!
const uint16_t missed_teeth = 2; // выбираем количество пропущенных зубьев
const bool CKPbeginLogic = 0;    // логика начала сигнала колена  (0 или 1)
#define CKPpin PB4               // пин коленвала (8 пин ардуино) можно менять
uint32_t T1, T2;

void setup()
{
    attachInterrupt(0, deteccion, RISING); // аппаратное прерывание по событию
    Serial.begin(115200);                  // инициализация порта вывода информации на экран
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
    Serial.println(T1 * 0.000001); // вывод времени прерывания в секундах
    if (Serial.available())
    {
        byte inbyte = Serial.read();
        if (inbyte >= '1' && inbyte <= '9') // в консоль вводим от 1 до 9, чтоб выбрать обороты(без символов конца строки)
        {
            uint32_t rpm = (inbyte - '0') * 1000;
            Serial.print(rpm);
            Serial.println(" rpm");
            OCR1A = 2666666UL / rpm;
        }
    }
}

void deteccion()
{
    T1 = micros();
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