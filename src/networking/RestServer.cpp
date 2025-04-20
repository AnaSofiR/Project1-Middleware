#include "../../include/networking/RestServer.hpp"
#include "../../include/core/MessageHandler.hpp"
#include <iostream>

RestServer::RestServer() {}

void RestServer::start(int port) {
  crow::SimpleApp app;

  // Init config and DB connection
  Config config("src/config.env");
  auto dbConn =
      std::make_shared<pqxx::connection>(config.getDBConnectionString());
  AuthService authService(config, dbConn);

  TopicManager topicManager;
  QueueManager queueManager;

  std::vector<std::string> peers;
  ReplicationManager replicationManager(peers);
 
  MessageHandler messageHandler(topicManager, queueManager, replicationManager);

  
  // Register endpoint
  CROW_ROUTE(app, "/register")
      .methods("POST"_method)([&authService](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) {
          return crow::response(400, "Invalid JSON");
        }

        try {
          std::string username = body["username"].s();
          std::string password = body["password"].s();
          authService.registerUser(username, password);
          return crow::response(200, "User registered");
        } catch (const std::exception &e) {
          return crow::response(500, e.what());
        }
      });

  // Login endpoint
  CROW_ROUTE(app, "/login")
      .methods("POST"_method)([&authService](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) {
          return crow::response(400, "Invalid JSON");
        }

        try {
          std::string username = body["username"].s();
          std::string password = body["password"].s();
          std::string token = authService.loginUser(username, password);
          crow::json::wvalue res;
          res["token"] = token;
          return crow::response(200, res);
        } catch (const std::exception &e) {
          return crow::response(401, e.what());
        }
      });
  
  // Receive message endpoint
  CROW_ROUTE(app, "/receive/<string>")
      .methods("GET"_method)([&messageHandler](const crow::request &req, const std::string &queue) {
        try {
          // Last parameter is the queue name
          std::string message = messageHandler.receiveFromQueue(queue, "usuario");  // Cambia "usuario" si es necesario
          if (message.empty()) {
            return crow::response(404, "No messages in the queue");
          } else {
            crow::json::wvalue res;
            res["message"] = message;
            return crow::response(200, res);
          }
        } catch (const std::exception &e) {
          return crow::response(500, e.what());
        }
      });
    
  // Send message endpoint
CROW_ROUTE(app, "/send/<string>")
  .methods("POST"_method)([&messageHandler](const crow::request &req, const std::string &queue) {
    auto body = crow::json::load(req.body);
    if (!body || !body.has("msm")) {
      return crow::response(400, "JSON inválido o falta el campo 'msm'");
    }

    try {
      std::string mensaje = body["msm"].s();
      std::string usuario = "usuario"; 

      messageHandler.sendToQueue(queue, mensaje, usuario);
      return crow::response(200, "Mensaje enviado correctamente");
    } catch (const std::exception &e) {
      return crow::response(500, e.what());
    }
  });

  // Send to topic endpoint
CROW_ROUTE(app, "/sendTopic/<string>")
  .methods("POST"_method)([&messageHandler](const crow::request &req, const std::string &topic) {
    auto body = crow::json::load(req.body);
    if (!body || !body.has("msm")) {
      return crow::response(400, "JSON inválido o falta el campo 'msm'");
    }

    try {
      std::string mensaje = body["msm"].s();
      std::string usuario = "usuario"; // Como placeholder por ahora

      messageHandler.sendToTopic(topic, mensaje, usuario);
      return crow::response(200, "Mensaje enviado al tópico correctamente");
    } catch (const std::exception &e) {
      return crow::response(500, e.what());
    }
});

// Receive from topic endpoint
CROW_ROUTE(app, "/receiveTopic/<string>")
    .methods("GET"_method)([&messageHandler](const crow::request &req, const std::string &topic) {
      try {
        std::string usuario = "usuario"; // Placeholder 
        auto mensajes = messageHandler.receiveFromTopic(topic, usuario);

        crow::json::wvalue res;
        res["mensajes"] = crow::json::wvalue::list(mensajes.begin(), mensajes.end());
        return crow::response(200, res);
      } catch (const std::exception &e) {
        return crow::response(500, e.what());
      }
});

// List topics endpoint
CROW_ROUTE(app, "/topics")
    .methods("GET"_method)([&topicManager](const crow::request &req) {
      try {
        auto topics = topicManager.listTopics();  // Usamos listTopics() aquí

        crow::json::wvalue res;
        res["topics"] = crow::json::wvalue::list(topics.begin(), topics.end());
        return crow::response(200, res);
      } catch (const std::exception &e) {
        return crow::response(500, e.what());
      }
});


  app.port(8080).multithreaded().run();
}

