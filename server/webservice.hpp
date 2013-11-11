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
        BusinessLogic::item_add(database, session, request);
        // TODO: return created planitem
        fcout << "\t\"data\": null\n";
        return 200;
    }

    unsigned items(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        // TODO: handle limit
        (void)request;
        std::vector<Item> items;
        Item::find_all(database, session.user(), items);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &item : items)
        {
            count++;
            fcout << "\t\t{\n";
            fcout << "\t\t\t\"id\": \"" << item.id() << "\",\n";
            fcout << "\t\t\t\"create\": \"/Date(" << (uint64_t)item.create()*1000 << ")/\",\n";
            fcout << "\t\t\t\"modify\": \"/Date(" << (uint64_t)item.modify()*1000 << ")/\",\n";
            fcout << "\t\t\t\"categoryid\": \"" << item.category().id() << "\",\n";
            fcout << "\t\t\t\"amount\": \"" << item.amount() << "\",\n";
            fcout << "\t\t\t\"description\": \"";
                jsonescape(item.description(), fcout);
                fcout << "\"\n";
            fcout << "\t\t}";
            if (count < items.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    unsigned planitem_add(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::planitem_add(database, session, request);
        // TODO: return created planitem
        fcout << "\t\"data\": null,\n";
        return 200;
    }

    unsigned planitems(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        // TODO: handle limit
        (void)request;
        std::vector<PlanItem> planitems;
        PlanItem::find_all(database, session.user(), planitems);
        fcout << "\t\"data\": [\n";
        unsigned count = 0;
        for (auto &item : planitems)
        {
            count++;
            fcout << "\t\t{\n";
            fcout << "\t\t\t\"id\": \"" << item.id() << "\",\n";
            fcout << "\t\t\t\"create\": \"/Date(" << (uint64_t)item.create()*1000 << ")/\",\n";
            fcout << "\t\t\t\"modify\": \"/Date(" << (uint64_t)item.modify()*1000 << ")/\",\n";
            fcout << "\t\t\t\"categoryid\": \"" << item.category().id() << "\",\n";
            fcout << "\t\t\t\"amount\": \"" << item.amount() << "\",\n";
            fcout << "\t\t\t\"description\": \"";
                jsonescape(item.description(), fcout);
                fcout << "\"\n";
            fcout << "\t\t}";
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
            fcout << "\t\t{\n";
            fcout << "\t\t\t\"id\": \"" << category.id() << "\",\n";
            fcout << "\t\t\t\"create\": \"/Date(" << (uint64_t)category.create()*1000 << ")/\",\n";
            fcout << "\t\t\t\"modify\": \"/Date(" << (uint64_t)category.modify()*1000 << ")/\",\n";
            fcout << "\t\t\t\"name\": \"";
                jsonescape(category.name(), fcout);
                fcout << "\",\n";
            fcout << "\t\t\t\"description\": \"";
                jsonescape(category.description(), fcout);
                fcout << "\"\n";
            fcout << "\t\t}";
            if (count < categories.size()) fcout << ",";
            fcout << std::endl;
        }
        fcout << "\t],\n";
        return 200;
    }

    unsigned category_add(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::category_add(database, session, request);
        // TODO: return created planitem
        fcout << "\t\"data\": null,\n";
        return 200;
    }

    unsigned category_edit(Database &database, Session &session,
            Request &request, std::ostream &fcout)
    {
        BusinessLogic::category_edit(database, session, request);
        // TODO: return changed planitem
        fcout << "\t\"data\": null,\n";
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
            else if (query == "webservice/planitems")
                status = planitems(database, session, request, fcout);
            else if (query == "webservice/planitem_add")
                status = planitem_add(database, session, request, fcout);
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
            else
            {
                fcout << "\t\"data\": null\n";
                status = 404;
            }
            if (status == 200)
                fcout << "\t\"sucess\": true,\n";
            else
                fcout << "\t\"sucess\": false,\n";
            fcout << "\t\"status\": " << status << "\n";
        } catch (BusinessLogic::MethodNotAllowed const &error)
        {
            fcout << "\t\"sucess\": false,\n";
            fcout << "\t\"status\": 405,\n";
            fcout << "\t\"data\": null\n";
        } catch (BusinessLogic::AccessDenied const &error)
        {
            fcout << "\t\"sucess\": false,\n";
            fcout << "\t\"status\": 405,\n";
            fcout << "\t\"data\": null\n";
        } catch (BusinessLogic::MalformedRequest const &error)
        {
            fcout << "\t\"sucess\": false,\n";
            fcout << "\t\"status\": 400,\n";
            fcout << "\t\"data\": null\n";
        } catch (...)
        {
            fcout << "\t\"sucess\": false,\n";
            fcout << "\t\"status\": 500,\n";
            fcout << "\t\"data\": null\n";
        }
        fcout << "}";
    }

}

#endif
