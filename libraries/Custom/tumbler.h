#pragma once

#include <arduino.h>

namespace custom {

class Tumbler 
{
public:
  enum State
  {
	Off = 0,
	On  = 1
  };

  Tumbler(int pinNumber, bool highIsOff = false); // highIsOff - признак того, что HIGH на пине
                                                  // соответствует низкому уровню
  ~Tumbler();
  
  void init(); // Вызывается в setup()!!!  
  
  int getPinNumber() const;
  State getCurrentState(); // Обновляет состояние тумблера и отдает его
    
private:
  int m_number;
  bool m_highIsOff;
  State m_currentState;
};

} // namespace custom

custom::Tumbler::Tumbler(int pinNumber, bool highIsOff)
  : m_number(pinNumber)
  , m_highIsOff(highIsOff)
  , m_currentState(Off)
{
}

custom::Tumbler::~Tumbler()
{
}

void custom::Tumbler::init()
{
  pinMode(m_number, INPUT);
}

int custom::Tumbler::getPinNumber() const
{
  return m_number;
}

custom::Tumbler::State custom::Tumbler::getCurrentState()
{
  if (m_highIsOff)
    m_currentState = digitalRead(m_number) == HIGH ? Off : On;
  else
    m_currentState = digitalRead(m_number) == HIGH ? On : Off;
	
  return m_currentState;
}