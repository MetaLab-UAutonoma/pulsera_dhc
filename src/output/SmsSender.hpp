#pragma once
#include "IMessageSender.hpp"
#include "../Sim7600Modem.hpp"

class SmsSender : public IMessageSender {
public:
    SmsSender(Sim7600Modem& modem) : modem_(modem) {}

    virtual bool send(const String& msg) override;

private:
    Sim7600Modem& modem_;
};