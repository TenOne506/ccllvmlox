#include "frontend/Ast.h"
#include "Error/Error.h"
#include <unordered_map>
#include <vector>
class Resolver {
        enum class ClassType {
            NONE,
            CLASS,
            SUBCLASS
        };

        using Scope = std::unordered_map<std::string_view, bool>;
        std::vector<Scope> scopes;
        LoxFunctionType currentFunction = LoxFunctionType::NONE;
        ClassType currentClass = ClassType::NONE;

        void beginScope() ;

        void endScope() ;

        void declare(const Token &name);

        void define(const Token &name);
        void resolveLocal(const Assignable &expr, const Token &name) const;

        void resolveFunction(const FunctionStmtPtr &function, const LoxFunctionType functionType) ;

    public:
        void operator()(const BlockStmtPtr &blockStmt);

        void operator()(const FunctionStmtPtr &functionStmt);

        void operator()(const ExpressionStmtPtr &expressionStmt);

        void operator()(const PrintStmtPtr &printStmt) ;

        void operator()(const ReturnStmtPtr &returnStmt);

        void operator()(const VarStmtPtr &varStmt);

        void operator()(const WhileStmtPtr &whileStmt);

        void operator()(const IfStmtPtr &ifStmt);
        void operator()(const ClassStmtPtr &classStmt);

        void operator()(const AssignExprPtr &assignExpr) ;

        void operator()(const BinaryExprPtr &binaryExpr) ;

        void operator()(const CallExprPtr &callExpr);

        void operator()(const GetExprPtr &getExpr) ;
        void operator()(const SetExprPtr &setExpr);

        void operator()(const ThisExprPtr &thisExpr) const;
        void operator()(const SuperExprPtr &superExpr) const ;
        void operator()(const VarExprPtr &varExpr) ;

        void operator()(const GroupingExprPtr &groupingExpr) ;

        void operator()(const LiteralExprPtr &) const;

        void operator()(const LogicalExprPtr &logicalExpr) ;

        void operator()(const UnaryExprPtr &unaryExpr);

    private:
        void resolve(const std::optional<Expr> &opt);

        void resolve(const Expr &expr);

        void resolve(const Stmt &stmt) ;

    public:
        void resolve(const Program &program);
    };