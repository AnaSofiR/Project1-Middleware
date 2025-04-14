#include "../../include/networking/RestServer.hpp"

RestServer::RestServer() {}

void RestServer::setReplicationManager(std::shared_ptr<ReplicationManager> repl) {
    replicationManager_ = repl;
}

void RestServer::start(int port) {
  crow::SimpleApp app;
  
    Config config("src/config.env");
    auto dbConn =
        std::make_shared<pqxx::connection>(config.getDBConnectionString());
    AuthService authService(config, dbConn);

    // Rutas principales
    CROW_ROUTE(app, "/register").methods("POST"_method)([&authService](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        try {
            authService.registerUser(body["username"].s(), body["password"].s());
            return crow::response(200, "User registered");
        } catch (const std::exception &e) {
            return crow::response(500, e.what());
        }
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([&authService](const crow::request &req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        try {
            std::string token = authService.loginUser(body["username"].s(), body["password"].s());
            crow::json::wvalue res;
            res["token"] = token;
            return crow::response(200, res);
        } catch (const std::exception &e) {
            return crow::response(401, e.what());
        }
    });

    std::cout << "REST escuchando en puerto: " << port << std::endl;
    app.port(port).multithreaded().run(); 
}
