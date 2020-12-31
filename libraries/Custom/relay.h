#pragma once

#include <arduino.h>

namespace custom {

class Relay 
{
public:
  enum State
  {
	Off = 0,
	On  = 1
  };

  Relay(int pinNumber);
  ~Relay();
  
  void init(State state = Off); // Вызывается в setup()!!!  
  
  void switchTo(State state);
  int getPinNumber() const;
  State getCurrentState() const;
  
  static State toState(uint8_t value); // HIGH or LOW
  
private:
  int m_number;
  State m_currentState;
};

} // namespace custom

custom::Relay::Relay(int pinNumber)
  : m_number(pinNumber)
  , m_currentState(Off)
{
}

custom::Relay::~Relay()
{
}

void custom::Relay::init(State state)
{
  pinMode(m_number, OUTPUT);
  
  switchTo(state);
}

void custom::Relay::switchTo(State state)
{
  if (state == State::On)
    digitalWrite(m_number, HIGH);
  else
    digitalWrite(m_number, LOW);
}

int custom::Relay::getPinNumber() const
{
  return m_number;
}

custom::Relay::State custom::Relay::getCurrentState() const
{
  return m_currentState;
}

custom::Relay::State custom::Relay::toState(uint8_t value)
{
  if (value == HIGH)
    return State::On;

  return State::Off;
}
