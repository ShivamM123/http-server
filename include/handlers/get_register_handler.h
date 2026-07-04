#ifndef GET_REGISTER_HANDLER_H
#define GET_REGISTER_HANDLER_H

#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../database/database.h"

class GetRegisterHandler {
public:
    static HttpResponse handle(const HttpRequest& req, Database& db);
};

#endif // GET_REGISTER_HANDLER_H