// 引入前端解析器的头文件
#include "frontend/Parser.h"
// 引入前端词法单元的头文件
#include "frontend/Token.h"
// 引入错误处理的头文件
#include "Error/Error.h"
// 引入智能指针相关的头文件
#include <memory>
#include <optional>

/**
 * @brief 消耗一个指定类型的词法单元，如果当前词法单元类型匹配则前进到下一个词法单元，否则报错
 * 
 * @param type 期望的词法单元类型
 * @param message 当词法单元类型不匹配时显示的错误信息
 * @return Token 消耗的词法单元
 */
Token Parser::consume(TokenType type, std::string_view message) {
    // 检查当前词法单元是否为期望的类型
    if (check(type)) {
        // 如果是，则前进到下一个词法单元并返回当前词法单元
        return advance();
    }
    // 如果不是，则调用错误处理函数并传入当前词法单元和错误信息
    error(peek(), message);
}

/**
 * @brief 解析表达式，从相等性表达式开始
 * 
 * @return Expr 解析得到的表达式
 */
Expr Parser::expression() {
    // 调用相等性表达式解析函数
    return equality();
}

/**
 * @brief 获取当前位置的词法单元
 * 
 * @return Token 当前位置的词法单元
 */
Token Parser::peek() {
    // 返回当前位置的词法单元
    return tokens[current];
}

/**
 * @brief 获取前一个位置的词法单元
 * 
 * @return Token 前一个位置的词法单元
 */
Token Parser::previous() {
    // 返回前一个位置的词法单元
    return tokens[current - 1];
}

/**
 * @brief 检查是否已经到达词法单元序列的末尾
 * 
 * @return true 如果已经到达末尾
 * @return false 如果还未到达末尾
 */
bool Parser::isAtEnd() {
    // 检查当前词法单元的类型是否为文件结束符
    return peek().getType() == TokenType::LoxEOF;
}

/**
 * @brief 解析基本表达式，如字面量、括号表达式等
 * 
 * @return Expr 解析得到的基本表达式
 */
Expr Parser::primary() {
    // 检查是否为布尔值 false
    if (match<TokenType>(FALSE)) {
        // 如果是，则返回一个表示 false 的字面量表达式
        return std::make_unique<LiteralExpr>(false);
    }
    // 检查是否为布尔值 true
    if (match<TokenType>(TRUE)) {
        // 如果是，则返回一个表示 true 的字面量表达式
        return std::make_unique<LiteralExpr>(true);
    }
    // 检查是否为 nil
    if (match<TokenType>(NIL)) {
        // 如果是，则返回一个表示 nil 的字面量表达式
        return std::make_unique<LiteralExpr>(nullptr);
    }
    // 检查是否为数字或字符串字面量
    if (match<TokenType>(NUMBER, STRING)) {
        // 如果是，则返回一个表示该字面量的表达式
        return std::make_unique<LiteralExpr>(previous().getLiteral());
    }
    // 检查是否为左括号
    if (match<TokenType>(LEFT_PAREN)) {
        // 如果是，则递归解析括号内的表达式
        Expr expr = expression();
        // 消耗右括号，如果没有则报错
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        // 返回一个表示括号表达式的对象
        return std::make_unique<GroupingExpr>(std::move(expr));
    }
    // 如果都不是，则报错
    error(peek(), "Expect expression.");
}

/**
 * @brief 解析一元表达式，如 ! 或 -
 * 
 * @return Expr 解析得到的一元表达式
 */
Expr Parser::unary() {
    // 检查是否为 ! 或 -
    if (match<TokenType>(BANG, MINUS)) {
        // 获取操作符
        Token operator_ = previous();
        // 递归解析右操作数
        Expr right = unary();
        // 创建一个一元表达式对象
        auto unaryexpr = std::make_unique<UnaryExpr>(operator_, std::move(right));
        // 返回该对象
        return std::move(unaryexpr);
    }
    // 如果不是一元表达式，则解析基本表达式
    return primary();
}

/**
 * @brief 解析乘除表达式
 * 
 * @return Expr 解析得到的乘除表达式
 */
Expr Parser::factor() {
    // 解析一元表达式
    Expr expr = unary();
    // 循环处理连续的乘除操作
    while (match<TokenType>(SLASH, STAR)) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = unary();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
        // 更新当前表达式
        expr = std::move(binexpr);
    }
    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析加减表达式
 * 
 * @return Expr 解析得到的加减表达式
 */
Expr Parser::term() {
    // 解析乘除表达式
    Expr expr = factor();
    // 循环处理连续的加减操作
    while (match<TokenType>(MINUS, PLUS)) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = factor();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
        // 更新当前表达式
        expr = std::move(binexpr);
    }
    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析比较表达式，如 >, >=, <, <=
 * 
 * @return Expr 解析得到的比较表达式
 */
Expr Parser::comparison() {
    // 解析加减表达式
    Expr expr = term();
    // 循环处理连续的比较操作
    while (match<TokenType>(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = term();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
        // 更新当前表达式
        expr = std::move(binexpr);
    }
    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析相等性表达式，如 !=, ==
 * 
 * @return Expr 解析得到的相等性表达式
 */
Expr Parser::equality() {
    // 解析比较表达式
    Expr expr = comparison();
    // 循环处理连续的相等性操作
    while (match<TokenType>(BANG_EQUAL, EQUAL_EQUAL)) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = comparison();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
        // 更新当前表达式
        expr = std::move(binexpr);
    }
    // 返回最终的表达式
    return expr;
}

/**
 * @brief 检查当前词法单元是否匹配任意一个给定的类型，如果匹配则前进到下一个词法单元
 * 
 * @tparam Args 可变参数模板，表示多个词法单元类型
 * @param type 可变参数，表示多个词法单元类型
 * @return true 如果匹配到任意一个类型
 * @return false 如果没有匹配到任何类型
 */
template<typename... Args>
bool Parser::match(Args... type) {
    // 遍历所有给定的类型
    for (auto t: {type...}) {
        // 检查当前词法单元是否为该类型
        if (check(t)) {
            // 如果是，则前进到下一个词法单元
            advance();
            // 返回匹配成功
            return true;
        }
    }
    // 如果都不匹配，则返回匹配失败
    return false;
}

/**
 * @brief 检查当前词法单元是否为指定类型
 * 
 * @param type 要检查的词法单元类型
 */
bool Parser::check(TokenType type) {
    if (isAtEnd()) { return false; }
    return peek().getType() == type;
}


/**
 * @brief 前进到下一个词法单元，并返回前一个词法单元
 * 
 * 如果当前位置不是词法单元序列的末尾，则将当前位置指针加1。
 * 然后返回前一个位置的词法单元。
 * 
 * @return Token 前一个位置的词法单元
 */
Token Parser::advance() {
    // 检查是否到达词法单元序列的末尾，如果没有则移动到下一个词法单元
    if (!isAtEnd()) { current++; }
    // 返回前一个词法单元
    return previous();
}

/**
 * @brief 同步解析器的状态，跳过无效的词法单元直到找到合适的同步点
 * 
 * 当解析过程中遇到错误时，调用此函数来尝试恢复解析。
 * 它会不断前进到下一个词法单元，直到遇到分号或者特定的关键字（如CLASS、FUN等）。
 * 
 */
void Parser::synchronize() {
    // 前进到下一个词法单元
    advance();

    // 当未到达词法单元序列的末尾时，继续循环
    while (!isAtEnd()) {
        // 如果前一个词法单元是分号，则停止同步
        if (previous().getType() == SEMICOLON) { return; }
        // 根据当前词法单元的类型进行判断
        switch (peek().getType()) {
            // 如果是这些关键字，则停止同步
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
        }
        // 继续前进到下一个词法单元
        advance();
    }
}

/**
 * @brief 解析类声明语句
 * 
 * 该函数用于解析类声明，首先消耗类名标识符，然后检查是否有父类声明。
 * 如果有父类声明，会消耗父类名标识符并创建一个变量表达式表示父类。
 * 
 * @return ClassStmtPtr 解析得到的类声明语句的智能指针
 */
ClassStmtPtr Parser::classDeclaration() {
    // 消耗类名标识符，如果没有则报错
    auto name = consume(IDENTIFIER, "expetc class name.");
    // 用于存储父类的可选变量表达式
    std::optional<VarExprPtr> superclass;

    // 检查是否有父类声明
    if (match(LESS)) {
        // 消耗父类名标识符，如果没有则报错
        consume(IDENTIFIER, "expect superclass name.");
        // 创建一个变量表达式表示父类
        superclass = std::make_unique<VarExpr>(previous());
    }
    consume(LEFT_BRACE, "Expect '{' before class body.");

    std::vector<FunctionStmtPtr> methods;
    while (!check(RIGHT_BRACE) && !isAtEnd()) { methods.push_back(function(LoxFunctionType::METHOD)); }

    consume(RIGHT_BRACE, "Expect '}' after class body.");

    return std::make_shared<ClassStmt>(name, std::move(superclass), std::move(methods));
}

/**
 * @brief 解析变量声明语句
 * 
 * 该函数用于解析变量声明，首先消耗变量名标识符，然后检查是否有初始化表达式。
 * 如果有初始化表达式，则解析该表达式；否则使用空字面量作为初始化值。
 * 最后消耗分号表示变量声明结束。
 * 
 * @return VarStmtPtr 解析得到的变量声明语句的智能指针
 */
VarStmtPtr Parser::varDeclaration() {
    // 消耗变量名标识符，如果没有则报错
    const Token name = consume(IDENTIFIER, "expect variable name.");
    // 如果有等号，则解析初始化表达式；否则使用空字面量作为初始化值
    Expr initializer = match(EQUAL) ? expression() : std::make_unique<LiteralExpr>(nullptr);
    // 消耗分号，如果没有则报错
    consume(SEMICOLON, "expect ';' after variable declaration.");
    // 创建一个变量声明语句并返回其智能指针
    return std::make_unique<VarStmt>(name, std::move(initializer));
}

/**
 * @brief 解析代码块语句
 * 
 * 该函数用于解析代码块，代码块由一对花括号包围。
 * 它会不断解析声明语句，直到遇到右花括号或到达词法单元序列的末尾。
 * 
 * @return StmtList 解析得到的代码块内的声明语句列表
 */
StmtList Parser::block() {
    // 用于存储代码块内的声明语句
    llvm::SmallVector<Stmt> statements;
    // 当未遇到右花括号且未到达词法单元序列的末尾时，继续循环
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        // 尝试解析一个声明语句
        if (auto decl = declaration(); decl.has_value()) {
            // 如果解析成功，将声明语句添加到列表中
            statements.push_back(std::move(decl.value()));
        }
    }
    // 消耗右花括号，如果没有则报错
    consume(RIGHT_BRACE, "Expect '}' after block.");
    // 返回代码块内的声明语句列表
    return statements;
}

/**
 * @brief 解析函数声明语句
 * 
 * 该函数用于解析函数声明，包括函数名、参数列表和函数体。
 * 它会检查参数数量是否超过限制，并处理构造函数的特殊情况。
 * 
 * @param type 函数的类型，如普通函数或方法
 * @return FunctionStmtPtr 解析得到的函数声明语句的智能指针
 */
FunctionStmtPtr Parser::function(LoxFunctionType type) {
    // 根据函数类型确定函数的描述
    const auto *kind = type == LoxFunctionType::FUNCTION ? "function" : "method";
    // 消耗函数名标识符，如果没有则报错
    Token name = consume(IDENTIFIER, "Expect " + std::string(kind) + " name.");
    // 消耗左括号，如果没有则报错
    consume(LEFT_PAREN, "expect '(' after" + std::string(kind) + "name");
    // 用于存储函数的参数列表
    llvm::SmallVector<Token> parameters;
    // 如果当前词法单元不是右括号，则继续解析参数
    if (!check(RIGHT_PAREN)) {
        do {
            // 检查参数数量是否超过限制
            if (parameters.size() >= MAX_PARAMETERS) {
                // 如果超过限制，调用错误处理函数
                error(peek(), "Can't have more than " + std::to_string(MAX_PARAMETERS) + " parameters.");
            }
            // 消耗参数名标识符，如果没有则报错
            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
            // 如果有逗号，则继续解析下一个参数
        } while (match(COMMA));
    }
    // 消耗右括号，如果没有则报错
    consume(RIGHT_PAREN, "Expect ')' after parameters.");
    // 消耗左花括号，如果没有则报错
    consume(LEFT_BRACE, "Expect '{' before " + std::string(kind) + " body.");
    // 解析函数体
    StmtList body = block();
    // 创建一个函数声明语句并返回其智能指针
    return std::make_unique<FunctionStmt>(
        name,
        // 如果是方法且函数名是 "init"，则将函数类型设置为构造函数
        type == LoxFunctionType::METHOD && name.getLexeme() == "init" ? LoxFunctionType::INITIALIZER : type,
        std::move(parameters), body
    );
}

/**
 * @brief 解析声明语句，尝试解析不同类型的声明
 * 
 * 该函数会依次尝试解析类声明、变量声明和函数声明。
 * 如果在解析过程中遇到错误，会调用同步函数进行错误恢复。
 * 
 * @return std::optional<Stmt> 如果解析成功，返回解析得到的声明语句；否则返回空
 */
std::optional<Stmt> Parser::declaration() {
    // 尝试解析类声明
    if (match(CLASS)) {
        // 如果匹配到 CLASS 关键字，则调用 classDeclaration 函数解析类声明
        return classDeclaration();
    }
    // 如果不是类声明，则尝试解析变量声明
    if (match(VAR)) {
        // 如果匹配到 VAR 关键字，则调用 varDeclaration 函数解析变量声明
        return varDeclaration();
    }
    // 如果既不是类声明也不是变量声明，则尝试解析函数声明
    if (match(FUN)) {
        // 如果匹配到 FUN 关键字，则调用 function 函数解析函数声明
        return function(LoxFunctionType::FUNCTION);
    }

    // 如果在解析过程中遇到错误
    if (hadError) {
        // 调用同步函数，跳过无效的词法单元直到找到合适的同步点
        synchronize();
        // 返回一个空的声明语句
        return std::make_optional<Stmt>();
    }
}

/**
 * @brief 解析整个程序，将所有声明语句添加到程序对象中
 * 
 * 该函数会不断调用 declaration 函数解析声明语句，直到到达词法单元序列的末尾。
 * 解析得到的声明语句会被添加到程序对象的声明列表中。
 * 
 * @return Program 解析得到的程序对象
 */
Program Parser::Parse() {
    // 创建一个新的程序对象
    auto program = Program();
    // 当未到达词法单元序列的末尾时，继续循环
    while (!isAtEnd()) {
        // 尝试解析一个声明语句
        if (auto decl = declaration()) {
            // 如果解析成功，将声明语句添加到程序对象的声明列表中
            program.push_back(std::move(decl.value()));
        }
    }
    // 返回解析得到的程序对象
    return program;
}