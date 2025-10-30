#include "subject_details_servlet.h"
#include <chen/log/log.h>

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

SubjectDetailsServlet::SubjectDetailsServlet()
    : BlogLoginedServlet("SubjectDetailsServlet") {
}

int32_t SubjectDetailsServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

}
}
