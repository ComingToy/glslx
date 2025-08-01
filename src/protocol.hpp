#ifndef __GLSLX_PROTOCOL_HPP__
#define __GLSLX_PROTOCOL_HPP__
#include "nlohmann/json.hpp"
#include "workspace.hpp"

class Protocol {
    Workspace workspace_;
    bool init_ = false;

    void make_response_(nlohmann::json& req, nlohmann::json* result);
    void initialize_(nlohmann::json& body);
    void did_open_(nlohmann::json& req);
    void definition_(nlohmann::json& req);
    void did_change_(nlohmann::json& req);
    void did_save_(nlohmann::json& req);
    void completion_(nlohmann::json& req);
    void document_symbol_(nlohmann::json& req);
    void semantic_token_(nlohmann::json& req);

    void send_to_client_(nlohmann::json& content);
    void publish_(std::string const& method, nlohmann::json* content);
    void publish_diagnostics(const std::string& error);
    void publish_clear_diagnostics(const std::string& uri);

public:
    int handle(nlohmann::json& req);
};
#endif

