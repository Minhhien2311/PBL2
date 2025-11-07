#ifndef ACCOUNT_ADMIN_H
#define ACCOUNT_ADMIN_H

#include <string>
#include "Account.h"

class AccountAdmin : public Account {
public:
    AccountAdmin(const std::string& username,
                 const std::string& passwordPlain,
                 const std::string& fullName,
                 const std::string& phone,
                 const std::string& email);

    AccountAdmin() = default;

    static AccountAdmin fromRecordLine(const std::string& line);
    std::string toRecordLine() const;
};

#endif
