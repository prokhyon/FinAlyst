#ifndef WEBSERVICE_HPP_INCLUDED
#define WEBSERVICE_HPP_INCLUDED

#include <iostream>

#include "request.hpp"
#include "session.hpp"
#include "template_common.hpp"
#include "utils.hpp"
#include "bulogic.hpp"

namespace WebService
{

    unsigned item_add(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        Item c = BusinessLogic::item_add(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << ",\n";
        return 200;
    }

    unsigned items(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        std::string limitstr, offsetstr;
        uint64_t limit = 0, offset = 0;
        if (request.get("limit", limitstr))
        {
            if (limitstr.empty() || !parse_unsigned(limitstr, limit))
                throw BusinessLogic::MalformedRequest("Invalid limit!");
        }
        if (request.get("offset", offsetstr))
        {
            if (offsetstr.empty() || !parse_unsigned(offsetstr, offset))
                throw BusinessLogic::MalformedRequest("Invalid offset!");
        }
        std::vector<Item> items;
        Item::find_all(database, session.user(), items, limit, offset);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &item : items)
        {
            count++;
            item.to_json(fcout);
            if (count < items.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "],\n";
        return 200;
    }

    unsigned planitem_add(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        PlanItem c = BusinessLogic::planitem_add(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << "\t,";
        return 200;
    }

    unsigned planitems(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        std::string limitstr, offsetstr;
        uint64_t limit = 0, offset = 0;
        if (request.get("limit", limitstr))
        {
            if (limitstr.empty() || !parse_unsigned(limitstr, limit))
                throw BusinessLogic::MalformedRequest("Invalid limit!");
        }
        if (request.get("offset", offsetstr))
        {
            if (offsetstr.empty() || !parse_unsigned(offsetstr, offset))
                throw BusinessLogic::MalformedRequest("Invalid offset!");
        }
        std::vector<PlanItem> planitems;
        PlanItem::find_all(database, session.user(), planitems, limit, offset);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &item : planitems)
        {
            count++;
            item.to_json(fcout);
            if (count < planitems.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    unsigned categories(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        // TODO: handle limit
        (void)request;
        std::vector<Category> categories;
        Category::find_all(database, session.user(), categories);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &category : categories)
        {
            count++;
            category.to_json(fcout);
            if (count < categories.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    unsigned category_add(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        Category c = BusinessLogic::category_add(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << ",\n";
        return 200;
    }

    unsigned category_edit(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        Category c = BusinessLogic::category_edit(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << ",\n";
        return 200;
    }

    unsigned item_edit(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        Item c = BusinessLogic::item_edit(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << ",\n";
        return 200;
    }

    unsigned planitem_edit(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        PlanItem c = BusinessLogic::planitem_edit(database, session, request);
        fcout << "\t\"data\":\n";
        c.to_json(fcout);
        fcout << ",\n";
        return 200;
    }

    unsigned category_destroy(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::category_destroy(database, session, request);
        fcout << "\t\"data\": null,\n";
        return 200;
    }

    unsigned item_destroy(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::item_destroy(database, session, request);
        fcout << "\t\"data\": null,\n";
        return 200;
    }

    unsigned planitem_destroy(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::planitem_destroy(database, session, request);
        fcout << "\t\"data\": null,\n";
        return 200;
    }

    unsigned balance_stats(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        std::vector<Category::BalanceData> data;
        BusinessLogic::balance_stats(database, session, request, data);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &p : data)
        {
            count++;
            fcout << "\t{\n";
            fcout << "\t\t\"interval\": " << p.interval << ",\n";
            fcout << "\t\t\"expensesum\": " << p.expensesum << ",\n";
            fcout << "\t\t\"plannedsum\": " << p.plannedsum << "\n";
            fcout << "\t}";
            if (count < data.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    unsigned daily_overview(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        std::vector<std::pair<Category, Category::BalanceData>> data;
        BusinessLogic::daily_overview(database, session, request, data);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &p : data)
        {
            count++;
            fcout << "\t{\n";
            fcout << "\t\t\"category\": ";
            p.first.to_json(fcout);
            fcout << ",\n";
            fcout << "\t\t\"interval\": " << p.second.interval << ",\n";
            fcout << "\t\t\"expensesum\": " << p.second.expensesum << ",\n";
            fcout << "\t\t\"plannedsum\": " << p.second.plannedsum << ",\n";
            fcout << "\t\t\"cumulative\": " << p.second.cumulative << "\n";
            fcout << "\t}";
            if (count < data.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    void handle_request(Database &database,
            Session &session,
            Request &request, std::ostream &fcout)
    {
        std::string query = request.query();
        html_content(fcout);
        fcout << "{\n";
        try {
            unsigned status = 500;
            if (query == "webservice/items")
                status = items(database, session, request, fcout);
            else if (query == "webservice/item_add")
                status = item_add(database, session, request, fcout);
            else if (query == "webservice/item_edit")
                status = item_edit(database, session, request, fcout);
            else if (query == "webservice/planitems")
                status = planitems(database, session, request, fcout);
            else if (query == "webservice/planitem_add")
                status = planitem_add(database, session, request, fcout);
            else if (query == "webservice/planitem_edit")
                status = planitem_edit(database, session, request, fcout);
            else if (query == "webservice/categories")
                status = categories(database, session, request, fcout);
            else if (query == "webservice/category_add")
                status = category_add(database, session, request, fcout);
            else if (query == "webservice/category_edit")
                status = category_edit(database, session, request, fcout);
            else if (query == "webservice/category_destroy")
                status = category_destroy(database, session, request, fcout);
            else if (query == "webservice/item_destroy")
                status = item_destroy(database, session, request, fcout);
            else if (query == "webservice/planitem_destroy")
                status = planitem_destroy(database, session, request, fcout);
            else if (query == "webservice/balance_stats")
                status = balance_stats(database, session, request, fcout);
            else if (query == "webservice/daily_overview")
                status = daily_overview(database, session, request, fcout);
            else
            {
                LOG_MESSAGE_WARN("Webservice error: 404 page not found");
                fcout << "\t\"data\": null,\n";
                status = 404;
            }
            if (status == 200)
                fcout << "\t\"success\": true,\n";
            else
                fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": " << status << "\n";
        } catch (BusinessLogic::MethodNotAllowed const &error)
        {
            LOG_MESSAGE_WARN("Webservice error: %s", error.what());
            fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": 405,\n";
            fcout << "\t\"data\": null\n";
        } catch (BusinessLogic::AccessDenied const &error)
        {
            LOG_MESSAGE_WARN("Webservice error: user %s acces denied: %s",
                    session.user().name().c_str(), error.what());
            fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": 405,\n";
            fcout << "\t\"data\": null\n";
        } catch (BusinessLogic::MalformedRequest const &error)
        {
            LOG_MESSAGE_WARN("Webservice error: %s", error.what());
            fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": 400,\n";
            fcout << "\t\"data\": null\n";
        } catch (std::exception const &error)
        {
            LOG_MESSAGE_WARN("Webservice unknown error: %s", error.what());
            fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": 500,\n";
            fcout << "\t\"data\": null\n";
        } catch (...)
        {
            LOG_MESSAGE_WARN("Webservice mysterious unknown error. This should never happen!");
            fcout << "\t\"success\": false,\n";
            fcout << "\t\"status\": 500,\n";
            fcout << "\t\"data\": null\n";
        }
        fcout << "}";
    }

}

#endif
