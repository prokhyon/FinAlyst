#include <iostream>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <cstdio>
#include <fcgio.h>
#include <gcrypt.h>

#include "logger.hpp"
#include "database.hpp"
#include "session.hpp"
#include "user.hpp"
#include "rand.hpp"
#include "request.hpp"
#include "template_common.hpp"
#include "webadmin.hpp"
#include "webservice.hpp"
#include "webgui.hpp"

using namespace std;

bool service_down = false;
std::string error_message;
SessionManager sessionman;

void parse_config(std::string const &filename, OptsMap &config)
{
    std::ifstream cf(filename);
    if (cf.is_open())
    {
        int lineno = 1;
        std::string line;
        while (std::getline(cf, line))
        {
            if (line.empty() || line[0] == '#') continue;
            size_t pos = line.find_first_of('=');
            if (pos == string::npos)
            {
                service_down = true;
                error_message = "Config file parse error on line " +
                    std::to_string(lineno) + ": " + line;
                return;
            }
            std::string key(line.substr(0, pos));
            std::string value(line.substr(pos+1, string::npos));
            config[key] = value;
            ++lineno;
        }
    } else
    {
        service_down = true;
        error_message = "Can't open config file: ";
        error_message += filename;
    }
}

void servicedown_page(OptsMap const &config, ostream &fcout)
{
    header(fcout, config, "Service down");
    fcout << "Error: " << error_message << "<br>";
    footer(fcout);
}

int main(void) {
    OptsMap config;
    FCGX_Request fcgi_request;

    FCGX_Init();
    FCGX_InitRequest(&fcgi_request, 0, 0);

    // Setup logging
    log_target_file("/var/log/finance.log");

    // Initialize application
    char const *config_file = getenv("CONFIG_FILE");
    if (config_file) parse_config(config_file, config);
    else {
        service_down = true;
        error_message = "CONFIG_FILE environment variable undefined";
    }

    Database database(
            config["database-server"], config["database-user"],
            config["database-pass"], config["database-name"]);
    if (!database.connected())
    {
        service_down = true;
        error_message = database.error_message();
    }

    // Accept requests
    while (FCGX_Accept_r(&fcgi_request) == 0) {
        fcgi_streambuf isbuf(fcgi_request.in);
        fcgi_streambuf osbuf(fcgi_request.out);
        fcgi_streambuf esbuf(fcgi_request.err);
        istream fcin(&isbuf);
        ostream fcout(&osbuf);
        ostream fcerr(&esbuf);

        if (service_down) servicedown_page(config, fcout);
        else
        {
            try
            {
                Request request(fcgi_request);
                Session *session = NULL;
                // Parse request
                std::string query = request.query();
                // Search for session cookie
                string sessionid = request.cookie("sessionid");
                if (sessionid.empty())
                {
                    sessionid = request.get("sessionid");
                    if (sessionid.empty() && request.type() == RequestType::Post)
                    {
                        sessionid = request.post("sessionid");
                    }
                }
                session = sessionman.session(sessionid);
                if (session != NULL) session->update_user();

                if (query.substr(0, sizeof("webservice/")-1) == "webservice/")
                {
                    if (query == "webservice/login")
                    {
                        webservice_login(database, sessionman,
                                request, fcout);
                    } else if (query == "webservice/logout")
                    {
                        webservice_logout(sessionman, session, fcout);
                    } else
                    {
                        if (session != NULL)
                        {
                            WebService::handle_request(
                                    database, *session, request, fcout);
                        } else
                        {
                            html_content(fcout);
                            fcout << "{\n";
                            fcout << "\t\"sucess\": false,\n";
                            fcout << "\t\"status\": 401\n";
                            fcout << "}";
                        }
                    }
                } else
                {
                    if (query == "logout")
                            logout_page(sessionman, session, fcout);
                    else if (query.empty() || query == "login" || session == NULL)
                    {
                        login_page(config, database, sessionman, session,
                                request, fcout);
                    } else 
                    {
                        if (session->user().administrator())
                            WebAdmin::handle_request(config,
                                    database, *session,
                                    request, fcout, fcin);
                        else
                        {
                            WebGUI::handle_request(config,
                                    database, *session,
                                    request, fcout);
                        }
                    }
                }
            }
            catch (std::logic_error const &error)
            {
                header(fcout, config, "Error");
                fcout << "Got exception: " << error.what() << std::endl;
            } catch (...)
            {
                header(fcout, config, "Error");
                fcout << "Got unknown exception" << std::endl;
            }
        }
        
        FCGX_Finish_r(&fcgi_request);
    }
    return 0;
}