#pragma once
#include <string>

/*
 * Send a generic HTTP error response over socket `fd`.
 *  - status: örn. 404, 405, 500
 *  - reason: örn. "Not Found", "Method Not Allowed", "Internal Server Error"
 *  - body:   hata sayfası gövdesi, örn. basit HTML
 */
void sendErrorResponse(int fd, int status, const std::string &reason, const std::string &body = "");

static void writeAll(int fd, const std::string &data);
