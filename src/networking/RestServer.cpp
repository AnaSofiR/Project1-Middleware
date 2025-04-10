#include "../../include/networking/RestServer.hpp"

int main() {
  crow::SimpleApp app;

  // Init config and DB connection
  Config config("src/config.env");
  auto dbConn =
      std::make_shared<pqxx::connection>(config.getDBConnectionString());
  AuthService authService(config, dbConn);

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

  app.port(8080).multithreaded().run();
}
