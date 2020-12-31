#pragma once

#include <arduino.h>

namespace custom {

class WaterLevelSensor
{
public:

  enum SensorState     // Состояние датчика уровня воды
  {                    
    LevelLow = 0,      // Уровень не достигнут
    LevelHigh          // Уровень достигнут
  };

  WaterLevelSensor(int pinNumber, int threshold); // Номер пина, порговое значение
  ~WaterLevelSensor();
  
  void readData();
  
  int getThreshold() const;
  SensorState getSensorState() const;
  int getSensorData() const;

private:
  int m_number;
  int m_threshold;
  SensorState m_currentState;
  int m_data;
};

} // namespace custom

custom::WaterLevelSensor::WaterLevelSensor(int pinNumber, int threshold)
  : m_number(pinNumber)
  , m_threshold(threshold)
  , m_currentState(LevelLow)
  , m_data(0)
{
}

custom::WaterLevelSensor::~WaterLevelSensor()
{
}

void custom::WaterLevelSensor::readData()
{
  m_data = analogRead(m_number);
  
  m_currentState = m_data <= m_threshold ? LevelLow : LevelHigh;
}

int custom::WaterLevelSensor::getThreshold() const
{
  return m_threshold;
}

custom::WaterLevelSensor::SensorState custom::WaterLevelSensor::getSensorState() const
{
  return m_currentState;
}

int custom::WaterLevelSensor::getSensorData() const
{
  return m_data;
}
