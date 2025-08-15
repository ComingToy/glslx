#ifndef __GLSLX_COMPUTE_INACTIVE_HPP__
#define __GLSLX_COMPUTE_INACTIVE_HPP__
#include <map>
#include <string>
#include <vector>

class ComputeInactiveHelper {

public:
    struct Range {
        int start;
        int end;
    };

    struct BlockAST {
        struct {
            int if_ = -1;
            std::vector<int> elif_;
            int el_ = -1;
            int endif_ = -1;
        } lines;

        std::vector<BlockAST*> then;
        std::vector<std::vector<BlockAST*>> elif;
        std::vector<BlockAST*> el;

        BlockAST() = default;

        BlockAST(std::vector<BlockAST*> then, std::vector<std::vector<BlockAST*>> elif, std::vector<BlockAST*> else_)
            : then(then), elif (elif), el(else_){};
        void release()
        {
            for (auto p : then) {
                p->release();
                delete p;
            }

            for (auto& v : elif) {
                for (auto p : v) {
                    p->release();
                    delete p;
                }
            }

            for (auto& v : elif) {
                for (auto* p : v) {
                    if (p)
                        p->release();
                    delete p;
                }
            }
        }
        ~BlockAST() { release(); }
    };

    ComputeInactiveHelper(std::vector<std::string> const& lines, std::map<int, int> const& cond);
    std::vector<Range> inactive();

private:
    struct _Token {
        enum class TokenKind { IF, ELSE, ELIF, ENDIF } kind;
        int line;
    };

    std::map<int, int> const& cond_;
    std::vector<_Token> inputs_;
    int pos_;
    BlockAST* parse_block();
    void eat();
    int line();
    bool match(const _Token::TokenKind token);
    std::vector<BlockAST*> parse();
    void compute_inactive(BlockAST* block, std::vector<Range>& result);
};
#endif
