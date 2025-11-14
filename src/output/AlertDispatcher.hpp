#pragma once
#include <vector>
#include <memory>
#include "senders/IMessageSender.hpp"
#include "../business/MeasurementManager.hpp"

class AlertDispatcher {
public:
    static AlertDispatcher& instance();

    void addSender(std::unique_ptr<IMessageSender> sender);
    void sendAlert(const MeasurementManager& mm);

private:
    AlertDispatcher() {}

    std::vector<std::unique_ptr<IMessageSender>> senders_;
};