#ifndef POST_LOGIN_HANDLER_H
#define POST_LOGIN_HANDLER_H

#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../database/database.h"

class PostLoginHandler {
public:
    static HttpResponse handle(const HttpRequest& req, Database& db);
};

#endif // POST_LOGIN_HANDLER_H