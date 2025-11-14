#pragma once
#include "../Sim7600Modem.hpp"
#include <Arduino.h>
#include <string>

#include "business/MeasurementManager.hpp"
#include "output/senders/IMessageSender.hpp"
#include "output/AlertMessageBuilder.hpp"

class SmsSender : public IMessageSender {
public:
    SmsSender(Sim7600Modem& modem) : modem_(modem) {}

    virtual bool send(const String& msg) override;

private:
    Sim7600Modem& modem_;
};