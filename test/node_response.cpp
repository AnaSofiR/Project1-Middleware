// responder.cpp
#include "../include/cluster/DiscoveryService.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  std::cout << "[Responder] Iniciando el modo respuesta en el puerto 8888 con "
               "respuesta en puerto 8080."
            << std::endl;

  // Inicia el responder para que esté escuchando y responda a mensajes
  // DISCOVER.
  startDiscoveryResponder(8888, 8080);

  // Mantiene el proceso activo durante 5 minutos para permitir pruebas.
  std::this_thread::sleep_for(std::chrono::minutes(5));

  std::cout << "[Responder] Finalizando." << std::endl;
  return 0;
}
