#ifndef POST_HOME_HANDLER_H
#define POST_HOME_HANDLER_H

#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../database/database.h"

class PostHomeHandler {
public:
    static HttpResponse handle(const HttpRequest& req, Database& db);
};

#endif // POST_HOME_HANDLER_H