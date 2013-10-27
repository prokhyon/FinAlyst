#ifndef PLAN_HPP_INCLUDED
#define PLAN_HPP_INCLUDED

#include <string>
#include <vector>

#include "database.hpp"
#include "user.hpp"
#include "category.hpp"

class Plan
{
public:

    Plan() :
        _database(NULL), _invalid(true),
        _id(std::numeric_limits<uint64_t>::max())
    {}

    Plan(Database &database, User const &user, Category const &category,
            uint64_t amount, std::string description) :
        _database(&database), _invalid(false), _detached(true),
        _userid(user.id()), _categoryid(category.id()),
        _amount(amount), _description(description)
    {
        time(&_create);
        _modify = _create;
    }

    Category category() { return Category(*_database, _categoryid); }
    User user() { return User(*_database, _userid); }

    uint64_t id() const { return _id; }
    time_t create() { return _create; }
    time_t modify() { return _modify; }
    uint64_t amount() { return _amount; }
    std::string description() { return _description; }

    void save()
    {
        if (_detached)
        {
            MYSQL_STMT* stmt = _database->statement(
                    "INSERT INTO `plan` (`create`, `modify`, `userid`, `categoryid`, `amount`, `description`) "
                    "VALUES (?, ?, ?, ?, ?, ?)");
            if (stmt == NULL)
                throw std::logic_error("Can't create statement");
            unsigned long size[6];

            MYSQL_TIME create;
            MYSQL_TIME modify;
            time_to_mysql(_create, create);
            time_to_mysql(_modify, modify);

            MYSQL_BIND param[6] = {
                mbind(create, size[0]),
                mbind(modify, size[1]),
                mbind(_userid, size[2]),
                mbind(_categoryid, size[3]),
                mbind(_amount, size[4]),
                mbind(_description, size[5]),
            };
            
            if (mysql_stmt_bind_param(stmt, param))
                throw std::logic_error(
                        std::string("Can't bind param in statement: ")
                        +mysql_stmt_error(stmt));
            if (mysql_stmt_execute(stmt))
                throw std::logic_error(
                        std::string("Can't execute statement: ")
                        +mysql_stmt_error(stmt));
        } else
        {
            // TODO: do update
        }
    }

    static void find_all(Database &database,
            User const &user, std::vector<Plan> &plans)
    {
        MYSQL_STMT* stmt = database.statement(
                    "SELECT `id`, `create`, `modify`, `amount`, `description` "
                    " FROM `plan` WHERE userid = ?");
        if (stmt == NULL)
            throw std::logic_error("Can't create statement");
        unsigned long size[5];
        uint64_t qid = 0;
        MYSQL_TIME create;
        MYSQL_TIME modify;
        uint64_t amount;
        char qdesc[256+1];
        
        MYSQL_BIND result[5] = {
            mbind(qid, size[0]),
            mbind(create, size[1]),
            mbind(modify, size[2]),
            mbind(amount, size[3]),
            mbind_fixed((char*)qdesc, size[4], sizeof(qdesc)),
        };
        static_assert(sizeof(size)/sizeof(size[0]) ==
                sizeof(result)/sizeof(result[0]),
                "result and size array should have the same length");

        uint64_t userid = user.id();
        unsigned long psize[1];
        MYSQL_BIND param[1] = {
            mbind(userid, psize[0])
        };
        
        if (mysql_stmt_bind_param(stmt, param))
                throw std::logic_error(
                        std::string("Can't bind param in statement: ")
                        +mysql_stmt_error(stmt));
        if (mysql_stmt_bind_result(stmt, result))
            throw std::logic_error(
                    std::string("Can't bind param in statement: ")
                    +mysql_stmt_error(stmt));
        if (mysql_stmt_execute(stmt))
            throw std::logic_error(
                    std::string("Can't execute statement: ")
                    +mysql_stmt_error(stmt));
        while (mysql_stmt_fetch (stmt) == 0)
        {
            plans.push_back(Plan(database, qid,
                        mysql_to_time(create), mysql_to_time(modify),
                        user.id(),
                        amount, std::string(qdesc)));
        }
    }

private:

    Plan(Database &database, uint64_t id, uint64_t userid,
            time_t create, time_t modify, 
            uint64_t amount, std::string description) :
        _database(&database), _detached(false), _id(id),
        _create(create), _modify(modify),
        _userid(userid), _amount(amount), _description(description)
    {}

    Database *_database;
    bool _invalid;
    bool _detached;

    uint64_t _id;
    time_t _create;
    time_t _modify;
    uint64_t _userid;
    uint64_t _categoryid;
    uint64_t _amount;
    std::string _description;
};

#endif