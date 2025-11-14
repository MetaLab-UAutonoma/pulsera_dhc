#include "AlertDispatcher.hpp"
#include "AlertMessageBuilder.hpp"

AlertDispatcher& AlertDispatcher::instance() {
    static AlertDispatcher inst;
    return inst;
}

void AlertDispatcher::addSender(std::unique_ptr<IMessageSender> sender) {
    senders_.push_back(std::move(sender));
}

void AlertDispatcher::sendAlert(const MeasurementManager& mm) {
    String smsMsg  = AlertMessageBuilder::buildSMS(mm);
    String jsonMsg = AlertMessageBuilder::buildJSON(mm);

    for (auto& s : senders_) {
        s->send(smsMsg);  // o jsonMsg según tipo de sender
    }
}