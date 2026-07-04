#ifndef ROUTER_H
#define ROUTER_H
#include <map>

#include <string>
#include <map>
#include <functional>
#include <utility>
#include "http_request.h"
#include "http_response.h"
#include "../database/database.h"

class Router {
public:
    // Every handler accepts a parsed request and a database reference, returning an HttpResponse
    using Handler = std::function<HttpResponse(const HttpRequest&, Database&)>;

    Router() = default;
    ~Router() = default;

    // Registers a specific HTTP Method and URL Path to an execution block
    void add_route(const std::string& method, const std::string& path, Handler handler);

    // Routes the parsed incoming request to its mapped handler
    HttpResponse handle_request(const HttpRequest& request, Database& db);

private:
    // Compound lookup key: std::pair<Method, Path> -> Mapped Execution Handler
    std::map<std::pair<std::string, std::string>, Handler> routes;

    // Standard 404 fallback for unmapped routes
    HttpResponse default_404_handler(const HttpRequest& request);
};

#endif // ROUTER_H