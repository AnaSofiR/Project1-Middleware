#include "core/MessageHandler.hpp"
#include <iostream>

MessageHandler::MessageHandler(TopicManager& tManager, QueueManager& qManager, ReplicationManager& rManager)
    : topicManager(tManager), queueManager(qManager), replicationManager(rManager) {}

void MessageHandler::sendToTopic(const std::string& topic, const std::string& message, const std::string& user) {
    if (!topicManager.publishMessage(topic, message)) {
        std::cerr << "[MessageHandler] Falló el envío al tópico '" << topic << "'.\n";
    } else {
        std::cout << "[MessageHandler] Usuario '" << user << "' envió mensaje a tópico '" << topic << "'.\n";
    }

    replicationManager.replicateTopic(topic, message);   
}

void MessageHandler::sendToQueue(const std::string& queue, const std::string& message, const std::string& user) {
    if (!queueManager.enqueueMessage(queue, message)) {
        std::cerr << "[MessageHandler] Falló el envío a la cola '" << queue << "'.\n";
    } else {
        std::cout << "[MessageHandler] Usuario '" << user << "' envió mensaje a cola '" << queue << "'.\n";
    }
}

std::vector<std::string> MessageHandler::receiveFromTopic(const std::string& topic, const std::string& user) {
    auto messages = topicManager.consumeMessages(topic);
    std::cout << "[MessageHandler] Usuario '" << user << "' recibió " << messages.size() << " mensajes del tópico '" << topic << "'.\n";
    return messages;
}

std::string MessageHandler::receiveFromQueue(const std::string& queue, const std::string& user) {
    auto msg = queueManager.dequeueMessage(queue);
    if (!msg.empty()) {
        std::cout << "[MessageHandler] Usuario '" << user << "' recibió mensaje de la cola '" << queue << "'.\n";
    }
    return msg;
}
