#ifndef __GLSLD_PROTOCOL_HPP__
#define __GLSLD_PROTOCOL_HPP__
#include "nlohmann/json.hpp"
#include "workspace.hpp"
#include <vector>

class Protocol {
    enum class SymbolKind {
        File = 1,
        Module = 2,
        Namespace = 3,
        Package = 4,
        Class = 5,
        Method = 6,
        Property = 7,
        Field = 8,
        Constructor = 9,
        Enum = 10,
        Interface = 11,
        Function = 12,
        Variable = 13,
        Constant = 14,
        String = 15,
        Number = 16,
        Boolean = 17,
        Array = 18,
        Object = 19,
        Key = 20,
        Null = 21,
        EnumMember = 22,
        Struct = 23,
        Event = 24,
        Operator = 25,
        TypeParameter = 26,
    };

    enum class ErrorCode {
        // Defined by JSON-RPC
        ParseError = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams = -32602,
        InternalError = -32603,

        /**
	 * This is the start range of JSON-RPC reserved error codes.
	 * It doesn't denote a real error code. No LSP error codes should
	 * be defined between the start and end range. For backwards
	 * compatibility the `ServerNotInitialized` and the `UnknownErrorCode`
	 * are left in the range.
	 *
	 * @since 3.16.0
	 */
        jsonrpcReservedErrorRangeStart = -32099,
        /** @deprecated use jsonrpcReservedErrorRangeStart */
        serverErrorStart = jsonrpcReservedErrorRangeStart,

        /**
	 * Error code indicating that a server received a notification or
	 * request before the server received the `initialize` request.
	 */
        ServerNotInitialized = -32002,
        UnknownErrorCode = -32001,

        /**
	 * This is the end range of JSON-RPC reserved error codes.
	 * It doesn't denote a real error code.
	 *
	 * @since 3.16.0
	 */
        jsonrpcReservedErrorRangeEnd = -32000,
        /** @deprecated use jsonrpcReservedErrorRangeEnd */
        serverErrorEnd = jsonrpcReservedErrorRangeEnd,

        /**
	 * This is the start range of LSP reserved error codes.
	 * It doesn't denote a real error code.
	 *
	 * @since 3.16.0
	 */
        lspReservedErrorRangeStart = -32899,

        /**
	 * A request failed but it was syntactically correct, e.g the
	 * method name was known and the parameters were valid. The error
	 * message should contain human readable information about why
	 * the request failed.
	 *
	 * @since 3.17.0
	 */
        RequestFailed = -32803,

        /**
	 * The server cancelled the request. This error code should
	 * only be used for requests that explicitly support being
	 * server cancellable.
	 *
	 * @since 3.17.0
	 */
        ServerCancelled = -32802,

        /**
	 * The server detected that the content of a document got
	 * modified outside normal conditions. A server should
	 * NOT send this error code if it detects a content change
	 * in its unprocessed messages. The result even computed
	 * on an older state might still be useful for the client.
	 *
	 * If a client decides that a result is not of any use anymore
	 * the client should cancel the request.
	 */
        ContentModified = -32801,

        /**
	 * The client has canceled a request and a server has detected
	 * the cancel.
	 */
        RequestCancelled = -32800,

        /**
	 * This is the end range of LSP reserved error codes.
	 * It doesn't denote a real error code.
	 *
	 * @since 3.16.0
	 */
        lspReservedErrorRangeEnd = -32800,
    };

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

    void send_to_client_(nlohmann::json& content);
    void publish_(std::string const& method, nlohmann::json* content);
    void publish_diagnostics(const std::string& error);
    void publish_clear_diagnostics(const std::string& uri);

public:
    int handle(nlohmann::json& req);
};
#endif


