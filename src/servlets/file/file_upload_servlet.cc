#include "file_upload_servlet.h"
#include <chen/log/log.h>
#include <chen/parser/multi_part_parser.h>
#include <chen/config/config.h>
#include <chen/util/util.h>
#include "../../manager/user_manager.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

static chen::ConfigVar<std::string>::ptr server_work_path = chen::Config::Lookup<std::string>("server.work_path");

FileUploadServlet::FileUploadServlet() 
	: BlogLoginedServlet("FileUploadServlet") {
}

int32_t FileUploadServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
		, chen::http::HttpSession::ptr session, Result::ptr result) {
	do {
		std::string content_type = request->getHeader("content-type");
		chen::MultipartParser::ptr parser = std::make_shared<chen::MultipartParser>(content_type);
		
		auto data = parser->parseToMemory(request->getBody());
		
		if (data.size() < 2) {
			result->setResult(400, "protocol error");
			break;
		}

        int64_t uid = getUserId(request);
        std::string user_name = UserMgr::GetInstance()->get(uid)->getName();
		std::string dir_name = data[0].content + "/" + user_name;
		std::string save_dir = server_work_path->getValue() + "/uploads/" + dir_name;

		for (size_t i = 1; i < data.size(); ++i) {
			std::string filename = save_dir + "/" + data[i].filename;
			std::ofstream ofs;
			bool rt = chen::FSUtil::OpenForWrite(ofs, filename, std::ios::binary);
			if (rt) {
				ofs.write(data[i].content.c_str(), data[i].content.size());
			}
			ofs.close();
			INFO(logger) << "File saved: " << filename << " (Size: " << data[i].content.size()
					<< " bytes -- " << (1.0 * data[i].content.size() / 1024)
					<< " kb -- " << (1.0 * data[i].content.size() / (1024 * 1024)) << " mb)";
		}
	} while (0);
	response->setBody(result->toJsonString());
    return 0;
}

} // namespace servlet
} // namespace blog
