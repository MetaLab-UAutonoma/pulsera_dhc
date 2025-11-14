#include "SmsSender.hpp"

bool SmsSender::send(const String& msg) {
    return modem_.sendSMS(msg);
}