#include "../../include/core/QueueManager.hpp"
#include <iostream>

bool QueueManager::createQueue(const std::string &queue,
                               const std::string &user) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (queueOwners.count(queue)) {
    std::cerr << "[QueueManager] La cola '" << queue << "' ya existe.\n";
    return false;
  }

  queueOwners[queue] = user;
  queueMessages[queue] = std::queue<std::string>();
  std::cout << "[QueueManager] Cola '" << queue << "' creada por '" << user
            << "'.\n";
  return true;
}

bool QueueManager::deleteQueue(const std::string &queue,
                               const std::string &user) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!queueOwners.count(queue)) {
    std::cerr << "[QueueManager] La cola '" << queue << "' no existe.\n";
    return false;
  }

  if (queueOwners[queue] != user) {
    std::cerr << "[QueueManager] Usuario '" << user
              << "' no tiene permiso para eliminar la cola '" << queue
              << "'.\n";
    return false;
  }

  queueOwners.erase(queue);
  queueMessages.erase(queue);
  std::cout << "[QueueManager] Cola '" << queue << "' eliminada por '" << user
            << "'.\n";
  return true;
}

std::vector<std::string> QueueManager::listQueues() {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<std::string> result;

  for (const auto &pair : queueOwners) {
    result.push_back(pair.first);
  }

  return result;
}

bool QueueManager::enqueueMessage(const std::string &queue,
                                  const std::string &message) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!queueMessages.count(queue)) {
    std::cerr << "[QueueManager] No se puede enviar mensaje. Cola '" << queue
              << "' no existe.\n";
    return false;
  }

  queueMessages[queue].push(message);
  std::cout << "[QueueManager] Mensaje encolado en '" << queue << "'.\n";
  return true;
}

std::string QueueManager::dequeueMessage(const std::string &queue) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!queueMessages.count(queue)) {
    std::cerr << "[QueueManager] Cola '" << queue << "' no existe.\n";
    return "";
  }

  if (queueMessages[queue].empty()) {
    std::cerr << "[QueueManager] Cola '" << queue << "' está vacía.\n";
    return "";
  }

  std::string msg = queueMessages[queue].front();
  queueMessages[queue].pop();
  std::cout << "[QueueManager] Mensaje desencolado de '" << queue << "'.\n";
  return msg;
}
