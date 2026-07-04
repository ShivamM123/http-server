#include "router.h"

using namespace std;

void Router::add_route(const string& method, const string& path, Handler handler) {
    routes[{method, path}] = move(handler);
}

HttpResponse Router::handle_request(const HttpRequest& request, Database& db) {
    string method = request.get_method();
    string path = request.get_path();

    auto it = routes.find({method, path});
    
    if (it != routes.end()) {
        return it->second(request, db);
    }

    return default_404_handler(request);
}

HttpResponse Router::default_404_handler(const HttpRequest& request) {
    HttpResponse response;
    response.set_status(404, "Not Found");
    response.add_header("Content-Type", "text/plain");
    response.set_body("404 Error: The requested endpoint \"" + request.get_path() + "\" does not exist on this server.");
    return response;
}