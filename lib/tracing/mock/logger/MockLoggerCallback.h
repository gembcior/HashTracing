#ifndef MOCK_LOGGER_CALLBACK_H
#define MOCK_LOGGER_CALLBACK_H

#include "logger/ILoggerCallback.h"

#include "gmock/gmock.h"

class MockLoggerCallback : public stm32::lib::ILoggerCallback
{
public:
  MOCK_METHOD(void, call, (), (override));
};

#endif /* MOCK_LOGGER_CALLBACK_H */
