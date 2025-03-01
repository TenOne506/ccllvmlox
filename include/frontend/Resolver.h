#include "frontend/Ast.h"
#include "Error/Error.h"
#include <unordered_map>
#include <vector>
/**
 * @brief 解析器类，用于解析代码中的作用域和变量声明
 * 
 * 该类负责管理代码中的作用域，包括开始和结束作用域，声明和定义变量，以及解析函数和局部变量。
 */
class Resolver {
    /**
     * @brief 表示当前所处的类类型的枚举
     * 
     * 该枚举定义了三种类类型：NONE 表示不在类中，CLASS 表示在普通类中，SUBCLASS 表示在子类中。
     */
    enum class ClassType {
        NONE,    // 不在类中
        CLASS,   // 在普通类中
        SUBCLASS // 在子类中
    };

    /**
     * @brief 作用域的类型定义
     * 
     * 作用域使用无序映射来表示，键为变量名，值为布尔值，表示变量是否已定义。
     */
    using Scope = std::unordered_map<std::string_view, bool>;

    // 作用域栈，用于管理嵌套的作用域
    std::vector<Scope> scopes={};

    // 当前函数的类型，初始为无函数类型
    LoxFunctionType currentFunction = LoxFunctionType::NONE;

    // 当前类的类型，初始为不在类中
    ClassType currentClass = ClassType::NONE;

    /**
     * @brief 开始一个新的作用域
     * 
     * 该函数向作用域栈中添加一个新的空作用域。
     */
    void beginScope();

    /**
     * @brief 结束当前作用域
     * 
     * 该函数从作用域栈中移除最后一个作用域。
     */
    void endScope();

    /**
     * @brief 声明一个变量
     * 
     * 该函数在当前作用域中声明一个变量，并将其标记为未定义。
     * 
     * @param name 变量的 Token 对象
     */
    void declare(const Token &name);

    /**
     * @brief 定义一个变量
     * 
     * 该函数在当前作用域中定义一个变量，即将其标记为已定义。
     * 
     * @param name 变量的 Token 对象
     */
    void define(const Token &name);

    /**
     * @brief 解析局部变量的作用域
     * 
     * 该函数确定局部变量在作用域栈中的位置。
     * 
     * @param expr 可赋值表达式，包含变量的作用域信息
     * @param name 变量的 Token 对象
     */
    void resolveLocal(const Assignable &expr, const Token &name) const;

    /**
     * @brief 解析函数声明
     * 
     * 该函数解析函数声明，包括参数和函数体，并管理函数的作用域。
     * 
     * @param function 函数声明的智能指针
     * @param functionType 函数的类型
     */
    void resolveFunction(const FunctionStmtPtr &function, const LoxFunctionType functionType);


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