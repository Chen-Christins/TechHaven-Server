#ifndef __BLOG_TYPES_H__
#define __BLOG_TYPES_H__

namespace blog {
namespace types {


/**
 * @brief 角色定义
 */
struct Role {
    enum class System {
        USER = 1,
        ADMIN = 2,
        EDITOR = 3,
        CHECKER = 4
    };

    enum class Organization {
        MEMBER = 1,
        ADMIN = 2,
        OWNER = 3
    };
};

/**
 * @brief 状态定义
 */
struct Status {
    enum class User {
        INACTIVE = 0,
        ACTIVE = 1,
        BANNED = 2
    };

    enum class Article {
        UNKNOWN = 0,
        CHECKING = 1,
        PUBLISHED = 2,
        REJECTED = 3,
        PRIVATE = 4
    };

    enum class Category {
        INACTIVE = 0,
        ACTIVE = 1,
    };

    enum class Assignment {
        INACTIVE = 0,
        ACTIVE = 1,
    };

    enum class Organization {
        INACTIVE = 0,
        ACTIVE = 1,
    };

    enum class UserOrganization {
        PENDING = 0,
        APPROVED = 1,
        REJECTED = 2,
        EXITED = 3
    };

    enum class EmailVerification {
        UNUSED = 0,
        USED = 1
    };
};

struct Type {
    enum class Article {
        ORIGINAL = 1,
        REPRINT = 2
    };
};

} // namespace types
} // namespace blog

#endif // __BLOG_TYPES_H__