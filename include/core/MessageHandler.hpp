#pragma once
#include <string>

class MessageHandler {
public:
    void sendToTopic(const std::string& topic, const std::string& message, const std::string& user);
    void sendToQueue(const std::string& queue, const std::string& message, const std::string& user);
    std::string receiveFromTopic(const std::string& topic, const std::string& user);
    std::string receiveFromQueue(const std::string& queue, const std::string& user);
};
