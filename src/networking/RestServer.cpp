#include "../../include/networking/RestServer.hpp"
#include "../../include/core/MessageHandler.hpp"
#include <iostream>

RestServer::RestServer() {

  topicManager_ = std::make_shared<TopicManager>();
  queueManager_ = std::make_shared<QueueManager>();
}

void RestServer::setReplicationManager(std::shared_ptr<ReplicationManager> repl) {
 

    replicationManager_ = repl;

}

void RestServer::start(int port) {
  crow::SimpleApp app;

  Config config("src/config.env");
  auto dbConn =
      std::make_shared<pqxx::connection>(config.getDBConnectionString());
  AuthService authService(config, dbConn);

  auto& topicManager = *topicManager_;
  auto& queueManager = *queueManager_;

  std::vector<std::string> peers;
  ReplicationManager replicationManager(peers);
 
  MessageHandler messageHandler(*topicManager_, *queueManager_, *replicationManager_);

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

      // Create queue endpoint
    CROW_ROUTE(app, "/createQueue").methods("POST"_method)([&queueManager](const crow::request& req){
      auto body = crow::json::load(req.body);

      // Validación del cuerpo de la solicitud
      if (!body || body["queue_name"].t() != crow::json::type::String || body["user"].t() != crow::json::type::String) {
          return crow::response(400, "Invalid request body. 'queue_name' and 'user' are required.");
      }

      std::string queue_name = body["queue_name"].s();
      std::string user = body["user"].s();

      if (queueManager.createQueue(queue_name, user)) {
          return crow::response(200, "Queue created successfully.");
      } else {
          return crow::response(400, "Queue already exists.");
      }
    });


  // create topic endpoint
    CROW_ROUTE(app, "/createTopic").methods("POST"_method)([&topicManager](const crow::request& req){
      auto body = crow::json::load(req.body);
      
      // Validación del cuerpo de la solicitud
      if (!body || body["topic_name"].t() != crow::json::type::String || body["user"].t() != crow::json::type::String) {
          return crow::response(400, "Invalid request body. 'topic_name' and 'user' are required.");
      }

      std::string topic_name = body["topic_name"].s();
      std::string user = body["user"].s();

      if (topicManager.createTopic(topic_name, user)) {
          return crow::response(200, "Topic created successfully.");
      } else {
          return crow::response(400, "Topic already exists.");
      }
    });

    //delete topic endpoint
  CROW_ROUTE(app, "/deleteTopic").methods("POST"_method)([&topicManager](const crow::request& req){
    auto body = crow::json::load(req.body);

    if (!body || body["topic_name"].t() != crow::json::type::String || body["user"].t() != crow::json::type::String) {
        return crow::response(400, "Invalid request body. 'topic_name' and 'user' are required.");
    }

    std::string topic_name = body["topic_name"].s();
    std::string user = body["user"].s();

    if (topicManager.deleteTopic(topic_name, user)) {
        return crow::response(200, "Topic deleted successfully.");
    } else {
        return crow::response(403, "You are not the owner of this topic or it doesn't exist.");
    }
  });
  
  // delete queue endpoint
  CROW_ROUTE(app, "/deleteQueue").methods("POST"_method)([&queueManager](const crow::request& req) {
    auto body = crow::json::load(req.body);

    if (!body || body["queue_name"].t() != crow::json::type::String || body["user"].t() != crow::json::type::String) {
        return crow::response(400, "Invalid request body. 'queue_name' and 'user' are required.");
    }

    std::string queue_name = body["queue_name"].s();
    std::string user = body["user"].s();

    if (queueManager.deleteQueue(queue_name, user)) {
        return crow::response(200, "Queue deleted successfully.");
    } else {
        return crow::response(403, "Queue deletion failed. You may not be the owner or the queue does not exist.");
    }
  });

  // list queues endpoint
  CROW_ROUTE(app, "/queues").methods("GET"_method)([&queueManager](const crow::request& req) {
    try {
        auto queues = queueManager.listQueues();
        crow::json::wvalue res;
        res["queues"] = crow::json::wvalue::list(queues.begin(), queues.end());
        return crow::response(200, res);
    } catch (const std::exception& e) {
        return crow::response(500, e.what());
    }
  });


  app.port(port).multithreaded().run();
}