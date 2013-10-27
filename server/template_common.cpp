#include "template_common.hpp"

void html_content(std::ostream &fcout)
{
    fcout << "Content-type: text/html; charset=utf-8\r\n"
         << "\r\n";
}

void header(std::ostream &fcout,
        OptsMap const &config, std::string const &title)
{
    auto it = config.find("asset-dir");
    std::string assetpath;
    if (it != config.end()) assetpath = it->second;
    html_content(fcout);
    fcout << "<!doctype html>";
    fcout << "<html>";
    fcout <<    "<head>";
    fcout <<        "<meta charset=\"UTF-8\">";
    fcout <<        "<title>" << title << " - Tiny Home Finance Application</title>\n";
    fcout <<        "<link rel=\"stylesheet\" type=\"text/css\" href=\""
        << assetpath << "/main.css\">";
    fcout <<        "<script src=\"" << assetpath << "/jquery.js\"></script>";
    fcout <<        "<script src=\"" << assetpath << "/jquery.browser.js\"></script>";
    fcout <<        "<script src=\"" << assetpath << "/jquery.iframe-auto-height.js\"></script>";
    fcout <<        "<script>";
    fcout <<            "$(document).ready(function () {";
    fcout <<                "$('iframe').iframeAutoHeight({debug: true});";
    fcout <<            "});";
    fcout <<        "</script>";
    fcout <<    "</head>\n";
    fcout <<    "<body>\n";
}

void footer(std::ostream &fcout)
{
    fcout << "  </body>\n"
          << "</html>\n";
}

void print_environment(std::ostream &fcout, Request const &request)
{
    fcout << "Cookies:<br>";
    request.each_cookie([&fcout] (std::string const &key, std::string const &value)
        { fcout << key << "=" << value << "<br>"; });
    fcout << "Environmental variables:<br>";
    request.each_env([&fcout] (std::string const &key, std::string const &value)
        { fcout << key << "=" << value << "<br>"; });
}