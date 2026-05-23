#include "rd_requirement_edit_servlet.h"
#include "rd_helper.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/requirement_manager.h"
#include "../../permission.h"
#include "../../util.h"

namespace blog {
namespace servlet {

RdRequirementEditServlet::RdRequirementEditServlet()
    : BlogLoginedServlet("RdRequirementEditServlet") {
}

int32_t RdRequirementEditServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        int64_t id = request->getParamAs<int64_t>("id", 0);
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");

        int64_t uid = getUserId(request);
        if (!uid) {
            result->setResult(500, "not login");
            break;
        }

        auto rel = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid);
        if (!rel || rel->getStatus() != OrganizationUserRelManager::Status::APPROVED) {
            result->setResult(403, "not a member of this organization");
            break;
        }
        int32_t orgRole = rel->getRole();

        bool is_new = false;
        data::RequirementInfo::ptr info;

        if (id) {
            info = RequirementMgr::GetInstance()->get(id);
            if (!info || info->getOrgId() != org_id) {
                result->setResult(404, "requirement not exist");
                break;
            }
            if (!permission::canEditRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
        } else {
            if (!permission::canCreateRequirement(orgRole)) {
                result->setResult(403, "Access Denied");
                break;
            }
            info.reset(new data::RequirementInfo);
            info->setOrgId(org_id);
            info->setCreatorId(uid);
            info->setCreateTime(time(0));
            is_new = true;
        }

        DEFINE_AND_CHECK_STRING(result, title, "title");
        if (!title.empty()) {
            info->setTitle(title);
        }
        
        std::string desc = request->getParam("description");
        if (!desc.empty()) {
            info->setDescription(desc);
        }

        std::string priorityStr = request->getParam("priority");
        if (!priorityStr.empty()) {
            int32_t p = rd::stringToPriority(priorityStr);
            if (p >= 0) {
                info->setPriority(p);
            }
        }

        std::string statusStr = request->getParam("status");
        if (!statusStr.empty()) {
            int32_t s = rd::stringToRequirementStatus(statusStr);
            if (s >= 0) {
                info->setStatus(s);
            }
        }

        int64_t assignee_id = request->getParamAs<int64_t>("assignee_id", 0);
        if (assignee_id) {
            info->setAssigneeId(assignee_id);
        }

        int64_t deadline = request->getParamAs<int64_t>("deadline", 0);
        if (deadline) {
            info->setDeadline(deadline);
        }

        std::string iteration = request->getParam("iteration");
        if (!iteration.empty()) {
            info->setIteration(iteration);
        }
        
        std::string category = request->getParam("category");
        if (!category.empty()) {
            info->setCategory(category);
        }

        std::string source = request->getParam("source");
        if (!source.empty()) {
            info->setSource(source);
        }

        info->setIsDeleted(0);
        info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::RequirementInfoDao::InsertOrUpdate(info, db)) {
            result->setResult(500, "insert or update requirement fail");
            break;
        }

        if (is_new) {
            RequirementMgr::GetInstance()->add(info);
        }

        rd::buildRequirementJson(result->jsondata, info);
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
