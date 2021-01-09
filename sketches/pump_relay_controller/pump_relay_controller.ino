#include <relay.h>
#include <waterLevelSensor.h>
#include <timer.h>
#include <tumbler.h>

#define THRESHOLD 1000
#define TIMER_40_MIN 2400000
#define TIMER_3_HOURS 10800000

bool pumpsSwitchOff();   // Выключить все насосы
bool systemSwitchOff();  // Выключить всю систему

// Переменные
bool isSystemOn = false; // Глобальная переменная для текущего состояния системы

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
Timer nightModeTimer(TIMER_3_HOURS);

// Реле
Relay pump2Relay(2);              // Реле насоса 2 
Relay pump3Relay(3);              // Реле насоса 3 
Relay pump4Relay(4);              // Реле насоса 4 
Relay pump5Relay(7);              // Реле насоса 5
Relay emptyPermFaultLampRelay(5); // Реле аварийной лампы пустого пермиата

// Тумблеры
Tumbler nightModeTumbler(8);  // Тумблер ночного режима
Tumbler systemWashTumbler(9); // Тумблер промывки системы (Насос 5)

void setup() 
{
  isSystemOn = true;
  
  pump2Relay.init();
  pump3Relay.init();
  pump4Relay.init();
  emptyPermFaultLampRelay.init();

  nightModeTumbler.init();
  systemWashTumbler.init();
}

void loop() 
{
  // Обновляем таймеры: если таймер не запущен или уже готов,
  // то ничего не произойдет
  pump3RelayTimer.update();
  nightModeTimer.update();
  
  // Читаем состояние управляющих тумблеров
  Tumbler::State nightModeTumblerState = nightModeTumbler.getCurrentState();
  Tumbler::State systemWashTumblerState = systemWashTumbler.getCurrentState();

  if (nightModeTumblerState == Tumbler::State::On &&
      systemWashTumblerState == Tumbler::State::On) {
    isSystemOn = true;
    nightModeTimer.reset();

    pumpsSwitchOff(); // Выключаем все насосы
    
    emptyPermFaultLampRelay.switchTo(Relay::State::On); // Включаем лампу пермиата
  }
  else if (nightModeTumblerState == Tumbler::State::On) {
    if (nightModeTimer.isFinished())
      systemSwitchOff();
    
    if (!nightModeTimer.isRunning() && isSystemOn) {
      nightModeTimer.reset();
      nightModeTimer.start();
    }

    if (isSystemOn) {
      pump2Relay.switchTo(Relay::State::On);
      pump3Relay.switchTo(Relay::State::Off);
      pump4Relay.switchTo(Relay::State::On);
      pump5Relay.switchTo(Relay::State::Off);

      emptyPermFaultLampRelay.switchTo(Relay::State::Off);
    }
  }
  else if (systemWashTumblerState == Tumbler::State::On) {
    isSystemOn = true;
    nightModeTimer.reset();

    pump2Relay.switchTo(Relay::State::Off);
    pump3Relay.switchTo(Relay::State::Off);
    pump4Relay.switchTo(Relay::State::Off);
    pump5Relay.switchTo(Relay::State::On);
    
    emptyPermFaultLampRelay.switchTo(Relay::State::Off);  
  }
  else { // Рядовая работа системы
    isSystemOn = true;
    nightModeTimer.reset();

    // Читаем показания датчиков
    bioMax.readData();
    bioMin.readData();
    concMax.readData();
    concMid.readData();
    concMin.readData();
    perMax.readData();
    perMin.readData();
    
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
      emptyPermFaultLampRelay.switchTo(Relay::State::Off);
    }
    else if (perMax.getSensorState() == SensorState::LevelHigh &&
             perMin.getSensorState() == SensorState::LevelHigh) {
      pump4Relay.switchTo(Relay::State::Off);
      emptyPermFaultLampRelay.switchTo(Relay::State::Off);     
    }
    else {
      // Обработка ошибки
    }
  }
    
  delay(300);
}

bool pumpsSwitchOff()
{
  pump2Relay.switchTo(Relay::State::Off); // Выключаем реле всех насосов
  pump3Relay.switchTo(Relay::State::Off);
  pump4Relay.switchTo(Relay::State::Off);
  pump5Relay.switchTo(Relay::State::Off);

  return true;
}

bool systemSwitchOff()
{
  pump3RelayTimer.reset(); // Сбрасываем все таймеры
  nightModeTimer.reset();

  pumpsSwitchOff();

  emptyPermFaultLampRelay.switchTo(Relay::State::Off);

  isSystemOn = false; // Обновляем флаг состояния системы

  return true;
}
