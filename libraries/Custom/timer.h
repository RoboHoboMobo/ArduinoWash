#pragma once

#include <arduino.h>

#include <math.h>

namespace custom {

class Timer
{
public:
  Timer(uint64_t delayMSec);
  ~Timer();
  
  bool start();
  bool update();
  void reset();
  
  bool isRunning() const;
  bool isFinished() const;

private:
  bool m_isStarted;
  bool m_isFinished;
  
  uint64_t m_beginTime;
  uint64_t m_passedTime;
  uint64_t m_delay;
};

} // namespace custom

custom::Timer::Timer(uint64_t delay)
  : m_isStarted(false)
  , m_isFinished(false)
  , m_beginTime(0)
  , m_passedTime(0)
  , m_delay(delay)
{
}

custom::Timer::~Timer()
{
}

bool custom::Timer::start()
{
  if (isRunning() || isFinished())
    return false;

  m_isStarted = true;  
  m_beginTime = millis();
  
  return true;
}

bool custom::Timer::update()
{
  if (!isRunning() || isFinished())
    return false;
  
  uint64_t temp = millis();                       // millis() возвращает unsigned long и может переполниться
  
  const bool wasOverflowed = m_passedTime > temp; // Признак переполнения счетчика
  if (wasOverflowed)
    temp += (uint32_t)(-1);                       // Обработка переполнения 
  
  m_passedTime = temp - m_beginTime;
  
  if (m_passedTime >= m_delay)
    m_isFinished = true;
  
  return true;
}

void custom::Timer::reset()
{
  m_isStarted = false;
  m_isFinished = false;
  
  m_beginTime = 0;
  m_passedTime = 0;
}

bool custom::Timer::isRunning() const
{
  return m_isStarted && !m_isFinished; 
}

bool custom::Timer::isFinished() const
{
  return m_isStarted && m_isFinished;
}