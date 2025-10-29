#include "file_upload_servlet.h"
#include <chen/log/log.h>
#include <chen/parser/multi_part_parser.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

FileUploadServlet::FileUploadServlet() 
	: BlogLoginedServlet("FileUploadServlet") {
}

int32_t FileUploadServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		std::string content_type = request->getHeader("content-type");
		chen::MultipartParser::ptr parser = std::make_shared<chen::MultipartParser>(content_type);

		if (!parser->parse(request->getBody(), "./uploads")) {
			result->setResult(301, "server unavailable");
			break;
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

} // namespace servlet
} // namespace blog
