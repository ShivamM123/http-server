#ifndef GET_HOME_HANDLER_H
#define GET_HOME_HANDLER_H

#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../database/database.h"

class GetHomeHandler {
public:
    static HttpResponse handle(const HttpRequest& req, Database& db);
};

#endif // GET_HOME_HANDLER_H