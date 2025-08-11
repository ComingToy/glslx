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

        BlockAST* then = nullptr;
        std::vector<BlockAST*> elif;
        BlockAST* el = nullptr;

        BlockAST() = default;

        BlockAST(BlockAST* then, std::vector<BlockAST*> elif, BlockAST* else_) : then(then), elif (elif), el(else_){};
        void release()
        {
            if (then)
                then->release();
            delete then;

            if (el)
                el->release();
            delete el;

            for (auto const& p : elif) {
                if (p)
                    p->release();
                delete p;
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
