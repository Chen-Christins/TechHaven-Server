#include "assignment_organization_create_servlet.h"
#include <chen/log/log.h>
#include "../../manager/user_manager.h"
#include "../../manager/organization_manager.h"
#include "../../manager/organization_user_rel_manager.h"
#include "../../manager/assignment_organization_rel_manager.h"
#include "../../manager/assignment_manager.h"
#include "../../util.h"
#include "../../permission.h"

namespace blog {
namespace servlet {

static chen::Logger::ptr logger = LOG_ROOT();

AssignmentOrganizationCreateServlet::AssignmentOrganizationCreateServlet()
    : BlogLoginedServlet("AssignmentOrganizationCreateServlet") {
}

int32_t AssignmentOrganizationCreateServlet::handle(chen::http::HttpRequest::ptr request, chen::http::HttpResponse::ptr response
        , chen::http::HttpSession::ptr session, Result::ptr result) {
    do {
        DEFINE_AND_CHECK_TYPE(result, int64_t, org_id, "org_id");
        DEFINE_AND_CHECK_STRING(result, name, "name");
        DEFINE_AND_CHECK_STRING(result, subject_name, "subject_name");
        DEFINE_AND_CHECK_TYPE(result, uint64_t, end_time, "end_time");
        DEFINE_AND_CHECK_TYPE(result, int32_t, max_size, "max_size");
        DEFINE_AND_CHECK_TYPE(result, int32_t, status, "status");
        DEFINE_AND_CHECK_TYPE(result, int32_t, priority, "priority");
        DEFINE_AND_CHECK_STRING(result, file_type, "file_type");
        DEFINE_AND_CHECK_STRING(result, description, "description");
        int64_t assign_id = request->getParamAs<int64_t>("assign_id", 0);

        // check operator permission
        auto uid = getUserId(request);
        std::string oper_name = UserMgr::GetInstance()->get(uid)->getName();
        int32_t system_role = UserMgr::GetInstance()->get(uid)->getRole();
        int32_t org_role = OrganizationUserRelMgr::GetInstance()->getByOrgAndUser(org_id, uid)->getRole();

        if (!permission::CanManageMembers(system_role, org_role)) {
            result->setResult(403, "Access Denied");
            break;
        }

        if (max_size > 96) {
            result->setResult(400, "max_size exceed limit");
            break;
        }

        // create or update assignment
        bool new_assignment = false;
        data::AssignmentInfo::ptr assign_info;
        if (assign_id) {
            assign_info = AssignmentMgr::GetInstance()->get(assign_id);
            if (!assign_info) {
                result->setResult(404, "assignment not exist");
                break;
            }
            assign_info->setName(name);
            assign_info->setSubjectName(subject_name);
        } else {
            assign_info = AssignmentMgr::GetInstance()->getByName(subject_name, name);
            if (!assign_info) {
                assign_info.reset(new data::AssignmentInfo);
                assign_info->setName(name);
                assign_info->setSubjectName(subject_name);
                assign_info->setCreateTime(time(0));
                new_assignment = true;
            } else if (assign_info->getIsDeleted()) {
                assign_info->setCreateTime(time(0));
            }
        }
        assign_info->setDeadline(end_time);
        assign_info->setMaxSize(max_size);
        assign_info->setStatus(status);
        assign_info->setPriority(priority);
        assign_info->setFileType(file_type);
        assign_info->setDescription(description);
        assign_info->setIsDeleted(0);
        assign_info->setUpdateTime(time(0));

        auto db = getDB();
        if (!db) {
            result->setResult(500, "get db error");
            break;
        }

        if (data::AssignmentInfoDao::InsertOrUpdate(assign_info, db)) {
            result->setResult(500, "insert or update assignment fail");
            ERROR(logger) << "db error, errno=" << db->getErrno()
                << " errstr=" << db->getErrStr();
            break;
        }

        if (new_assignment) {
            AssignmentMgr::GetInstance()->add(assign_info);

            // create organization assignment relation
            auto org_assign_rel = AssignmentOrganizationRelMgr::GetInstance()->getByOrgAndAssign(org_id, assign_info->getId());
            if (!org_assign_rel) {
                org_assign_rel.reset(new data::AssignmentOrganizationRelInfo);
                org_assign_rel->setOrganizationId(org_id);
                org_assign_rel->setAssignmentId(assign_info->getId());
                org_assign_rel->setCreateTime(time(0));
            }
            org_assign_rel->setAssignedBy(oper_name);
            org_assign_rel->setIsDeleted(0);
            org_assign_rel->setUpdateTime(time(0));

            if (data::AssignmentOrganizationRelInfoDao::InsertOrUpdate(org_assign_rel, db)) {
                result->setResult(500, "insert or update organization assignment rel fail");
                ERROR(logger) << "db error, errno=" << db->getErrno()
                    << " errstr=" << db->getErrStr();
                break;
            }

            AssignmentOrganizationRelMgr::GetInstance()->add(org_assign_rel);

            result->set("assign_id", assign_info->getId());
            result->set("assigned_by", org_assign_rel->getAssignedBy());
        }
        result->set("id", assign_info->getId());
        result->set("name", assign_info->getName());
        result->set("subject_name", assign_info->getSubjectName());
        result->set("end_time", assign_info->getDeadline());
        result->set("max_size", assign_info->getMaxSize());
        result->set("status", assign_info->getStatus());
        result->set("priority", assign_info->getPriority());
        result->set("file_type", assign_info->getFileType());
        result->set("description", assign_info->getDescription());
    } while (0);
    response->setBody(result->toJsonString());
    return 0;
}

}
}
