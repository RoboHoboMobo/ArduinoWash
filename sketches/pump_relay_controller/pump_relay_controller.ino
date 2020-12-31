#include <relay.h>
#include <waterLevelSensor.h>
#include <timer.h>

#define THRESHOLD 1000
#define TIMER_40_MIN 2400000

using namespace custom;
typedef WaterLevelSensor::SensorState SensorState;

// Датчики
WaterLevelSensor bioMax(0, THRESHOLD);  // Датчик уровня MAX биореактора
WaterLevelSensor bioMin(1, THRESHOLD);  // Датчик уровня MIN биореактора
WaterLevelSensor concMax(2, THRESHOLD); // Датчик уровня MAX концентрата
WaterLevelSensor concMid(3, THRESHOLD); // Датчик уровня MID концентрата
WaterLevelSensor concMin(4, THRESHOLD); // Датчик уровня MIN концентрата
WaterLevelSensor perMax(5, THRESHOLD);  // Датчик уровня MAX пермиата
WaterLevelSensor perMin(6, THRESHOLD);  // Датчик уровня MIN пермиата

// Таймеры
Timer pump3RelayTimer(TIMER_40_MIN);

// Реле
// Где реле для первого насоса?
Relay pump2Relay(2);              // Реле насоса 2 
Relay pump3Relay(3);              // Реле насоса 3 
Relay pump4Relay(4);              // Реле насоса 4 
Relay emptyPermFaultLampRelay(5); // Реле аварийной лампы пустого пермиата

void setup() 
{
  pump2Relay.init();
  pump3Relay.init();
  pump4Relay.init();
  emptyPermFaultLampRelay.init();
}

void loop() 
{
  // Читаем показания датчиков
  bioMax.readData();
  bioMin.readData();
  concMax.readData();
  concMid.readData();
  concMin.readData();
  perMax.readData();
  perMin.readData();
  
  // Обновляем таймеры
  if (pump3RelayTimer.isRunning())
    pump3RelayTimer.update();

  // Логика для реле второго насоса
  if (bioMax.getSensorState() == SensorState::LevelHigh &&
      bioMin.getSensorState() == SensorState::LevelLow) {
        // Обработка ошибки
  }
  else if (bioMax.getSensorState() == SensorState::LevelLow &&
           bioMin.getSensorState() == SensorState::LevelHigh) 
    pump2Relay.switchTo(Relay::State::On);
  else
    pump2Relay.switchTo(Relay::State::Off);

  // Логика для реле третьего насоса
  if (concMax.getSensorState() == SensorState::LevelLow &&
      concMid.getSensorState() == SensorState::LevelLow &&
      concMin.getSensorState() == SensorState::LevelHigh) {
    pump3Relay.switchTo(Relay::State::On);

    pump3RelayTimer.reset();
  }
  else if (concMax.getSensorState() == SensorState::LevelLow &&
           concMid.getSensorState() == SensorState::LevelHigh &&
           concMin.getSensorState() == SensorState::LevelHigh) {	
	pump3RelayTimer.isFinished() ? pump3Relay.switchTo(Relay::State::On) :
	                               pump3Relay.switchTo(Relay::State::Off);
								   
    if (!pump3RelayTimer.isRunning() && 
        pump3Relay.getCurrentState() == Relay::State::Off) { // Запустить таймер, если он
      pump3RelayTimer.reset();                               // еще не запущен и реле выкл
	  pump3RelayTimer.start();                             
    }
  }
  else if (concMax.getSensorState() == SensorState::LevelHigh &&
           concMid.getSensorState() == SensorState::LevelHigh &&
           concMin.getSensorState() == SensorState::LevelHigh) {
    pump3Relay.switchTo(Relay::State::Off);

    pump3RelayTimer.reset();
  }  
  else {
    // Обработка ошибок
  }
   
  // Логика для реле четвертого насоса
  if (perMax.getSensorState() == SensorState::LevelLow &&
      perMin.getSensorState() == SensorState::LevelLow) {
    pump4Relay.switchTo(Relay::State::On);
    emptyPermFaultLampRelay.switchTo(Relay::State::On);     
  }
  else if (perMax.getSensorState() == SensorState::LevelLow &&
           perMin.getSensorState() == SensorState::LevelHigh) {
    pump4Relay.switchTo(Relay::State::On);
    // emptyPermFaultLampRelay ???      
  }
  else if (perMax.getSensorState() == SensorState::LevelHigh &&
           perMin.getSensorState() == SensorState::LevelHigh) {
    pump4Relay.switchTo(Relay::State::Off);
    // emptyPermFaultLampRelay ???      
  }
  else {
    // Обработка ошибки
  }
    
  delay(300);
}
