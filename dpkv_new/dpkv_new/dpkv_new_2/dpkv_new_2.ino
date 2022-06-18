 #include <TimerOne.h>

#define EXTI_NUM  0 // номер прерывания
#define EXTI_PIN 2 // PD2, датчик шторки
#define OUT_PIN  12 // PB4, выходной сигнал

#define ISR_TO_RPM_RATIO 4 // прорезей в шторке на один оборот
#define TEETH_COUNT 60 // число зубов
#define TEETH_MISSED 2 // сколько зубов пропущено
#define TEETH_MISSED_PIN_STATE 0 // состояние выхода на пропущенных зубах (0, 1)
#define TEETH_START_OFFSET 20 // сдвиг первого зуба от начала сигнала шторки
#define TEETH_INIT_STATE 0 // состояние выхода на первом зубе (0, 1)

#define NO_ISR_TIMEOUT 500 // таймаут прерывания шторки, мс

static uint8_t teethStateArr[TEETH_COUNT * 2];
static volatile uint16_t currentTeethPos = 0;
static volatile uint8_t isrFired = 0, revolutionCounter = 0;
static uint32_t currentRpm = 0, targetRpm = 0;
static volatile uint32_t extiTime = 0, extiPrevTime = 0, extiPeriod = 0;

static void extiISR();
static void teethISR();

uint8_t state = 0;

void setup() {
  uint8_t teethInitState = TEETH_INIT_STATE;
  
  for (uint16_t i = 0; i < TEETH_START_OFFSET * 2; i++) {
    teethStateArr[i] = teethInitState;
    teethInitState = !teethInitState;
  }

  for (uint16_t i = TEETH_START_OFFSET * 2; i < TEETH_START_OFFSET * 2 + TEETH_MISSED * 2; i++) {
    teethStateArr[i] = TEETH_MISSED_PIN_STATE;
    teethInitState = !teethInitState;
  }

  for (uint16_t i = TEETH_START_OFFSET * 2 + TEETH_MISSED * 2; i < TEETH_COUNT * 2; i++) {
    teethStateArr[i] = teethInitState;
    teethInitState = !teethInitState;
  }

  Serial.begin(115200);

  pinMode(EXTI_PIN, INPUT_PULLUP);
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, TEETH_INIT_STATE);
  attachInterrupt(EXTI_NUM, extiISR, RISING); // прерывание по прохождению шторки, растущий фронт

  Timer1.initialize(500000);
  Timer1.attachInterrupt(teethISR);
}

void loop() {
  static uint8_t prevIsrState;
  static uint32_t lastIsrFired = 0, lastSerialUpdate = 0;
  
  if (isrFired != prevIsrState) {
    lastIsrFired = millis();

    prevIsrState = isrFired;
  }

  if (extiPeriod && extiPeriod <= 1000000) {
    currentRpm = (60000000 / ISR_TO_RPM_RATIO) / extiPeriod;
  }
  else {
    currentRpm = 0;
  }

  if (millis() - lastIsrFired > NO_ISR_TIMEOUT) {
    currentRpm = 0;
  }

  if (targetRpm < currentRpm) {
    targetRpm++;
  }
  else if (targetRpm > currentRpm) {
    targetRpm--;
  }

  if (millis() - lastSerialUpdate > 1000) {
    Serial.print("Current rpm: ");
    Serial.println(currentRpm);

    lastSerialUpdate = millis();
  }
}

static void extiISR() {
  extiTime = micros();

  revolutionCounter++;

  if (revolutionCounter >= ISR_TO_RPM_RATIO) {
    revolutionCounter = 0;
    currentTeethPos = 0;
  }

  extiPeriod = extiTime - extiPrevTime;

  isrFired++;

  extiPrevTime = extiTime;  
}

static void teethISR() {
  static uint32_t prevRpm = 0;
  
  if (targetRpm) {
    digitalWrite(OUT_PIN, teethStateArr[currentTeethPos]);

    currentTeethPos++;

    if (currentTeethPos >= TEETH_COUNT) {
      currentTeethPos = 0;
    }

    if (prevRpm != targetRpm) {
      Timer1.setPeriod((30000000 / targetRpm) / TEETH_COUNT);
    }
  }
  else {
    Timer1.setPeriod(500000);
  }
}
