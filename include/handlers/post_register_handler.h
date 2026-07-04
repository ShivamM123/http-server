#ifndef POST_REGISTER_HANDLER_H
#define POST_REGISTER_HANDLER_H

#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../database/database.h"

class PostRegisterHandler {
public:
    static HttpResponse handle(const HttpRequest& req, Database& db);
};

#endif // POST_REGISTER_HANDLER_H