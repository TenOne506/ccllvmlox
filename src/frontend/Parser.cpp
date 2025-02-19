// 引入前端解析器的头文件
#include "frontend/Parser.h"
// 引入前端词法单元的头文件
#include "frontend/Ast.h"
#include "frontend/Token.h"
// 引入错误处理的头文件
#include "Error/Error.h"
// 引入智能指针相关的头文件
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <optional>

/**
 * @brief 消耗一个指定类型的词法单元，如果当前词法单元类型匹配则前进到下一个词法单元，否则报错
 * 
 * @param type 期望的词法单元类型
 * @param message 当词法单元类型不匹配时显示的错误信息
 * @return Token 消耗的词法单元
 */
Token Parser::consume(TokenType type, std::string message) {
    // 检查当前词法单元是否为期望的类型
    if (check(type)) {
        // 如果是，则前进到下一个词法单元并返回当前词法单元
        return advance();
    }
    // 如果不是，则调用错误处理函数并传入当前词法单元和错误信息
    throw error(peek(), message);
}
/**
 * @brief 解析逻辑与表达式
 * 
 * 该函数从相等性表达式开始解析，然后处理连续的逻辑与操作。
 * 它会不断检查当前词法单元是否为 AND 关键字，如果是，则继续解析右操作数，并创建一个逻辑与表达式对象。
 * 
 * @return Expr 解析得到的逻辑与表达式
 */
Expr Parser::and_() {
    // 解析相等性表达式
    auto expr = equality();

    // 循环处理连续的逻辑与操作
    while (match(AND)) {
        // 解析右操作数
        auto right = equality();
        // 创建一个逻辑与表达式对象
        expr = std::make_unique<LogicalExpr>(std::move(expr), LogicalOp::AND, std::move(right));
    }

    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析逻辑或表达式
 * 
 * 该函数从逻辑与表达式开始解析，然后处理连续的逻辑或操作。
 * 它会不断检查当前词法单元是否为 OR 关键字，如果是，则继续解析右操作数，并创建一个逻辑或表达式对象。
 * 
 * @return Expr 解析得到的逻辑或表达式
 */
Expr Parser::or_() {
    // 解析逻辑与表达式
    auto expr = and_();

    // 循环处理连续的逻辑或操作
    while (match(OR)) {
        // 解析右操作数
        auto right = and_();
        // 创建一个逻辑或表达式对象
        expr = std::make_unique<LogicalExpr>(std::move(expr), LogicalOp::OR, std::move(right));
    }

    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析赋值表达式
 * 
 * 该函数从逻辑或表达式开始解析，如果遇到等号，则尝试解析赋值操作。
 * 它会检查左操作数是否为变量表达式或属性访问表达式，如果是，则创建一个赋值表达式对象；
 * 否则，抛出一个错误。
 * 
 * @return Expr 解析得到的赋值表达式
 */
Expr Parser::assignment() {
    // 调用或表达式解析函数
    auto expr = or_();
    // 检查是否为赋值操作
    if (match(EQUAL)) {
        // 获取等号对应的词法单元
        const auto equals = previous();
        // 递归解析赋值表达式的右操作数
        auto value = assignment();

        // 检查左操作数是否为变量表达式
        if (std::holds_alternative<VarExprPtr>(expr)) {
            // 获取变量名
            const auto name = std::get<VarExprPtr>(expr)->name;
            // 创建一个赋值表达式对象
            return std::make_unique<AssignExpr>(name, std::move(value));
        }
        // 检查左操作数是否为属性访问表达式
        if (std::holds_alternative<GetExprPtr>(expr)) {
            // 获取属性访问表达式
            const auto &getExpr = std::get<GetExprPtr>(expr);
            // 创建一个属性赋值表达式对象
            return std::make_unique<SetExpr>(std::move(getExpr->object), getExpr->name, std::move(value));
        }

        // 如果左操作数不是有效的赋值目标，则抛出错误
        error(equals, "Invalid assignment target.");
    }

    // 如果不是赋值操作，则返回逻辑或表达式
    return expr;
}

/**
 * @brief 解析表达式，从相等性表达式开始
 * 
 * @return Expr 解析得到的表达式
 */
Expr Parser::expression() {
    // 调用相等性表达式解析函数
    //return equality();
    return assignment();
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
    if (match(FALSE)) {
        // 如果是，则返回一个表示 false 的字面量表达式
        return std::make_unique<LiteralExpr>(false);
    }
    // 检查是否为布尔值 true
    if (match(TRUE)) {
        // 如果是，则返回一个表示 true 的字面量表达式
        return std::make_unique<LiteralExpr>(true);
    }
    // 检查是否为 nil
    if (match(NIL)) {
        // 如果是，则返回一个表示 nil 的字面量表达式
        return std::make_unique<LiteralExpr>(nullptr);
    }
    // 检查是否为数字或字符串字面量
    if (match({NUMBER, STRING})) {
        // 如果是，则返回一个表示该字面量的表达式
        return std::make_unique<LiteralExpr>(previous().getLiteral());
    }
    // 检查是否为左括号
    if (match(LEFT_PAREN)) {
        // 如果是，则递归解析括号内的表达式
        Expr expr = expression();
        // 消耗右括号，如果没有则报错
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        // 返回一个表示括号表达式的对象
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match(THIS)) { return std::make_unique<ThisExpr>(previous()); }

    if (match(SUPER)) {
        Token keyword = previous();
        consume(DOT, "Expect '.' after 'super'.");
        Token method = consume(IDENTIFIER, "Expect superclass method name.");
        return std::make_unique<SuperExpr>(keyword, method);
    }
    // 如果都不是，则报错
    throw error(peek(), "Expect expression.in prarser cpp 208 line");
}
/**
 * @brief 完成函数调用表达式的解析
 * 
 * 该函数用于解析函数调用的参数列表，并最终创建一个 CallExpr 对象表示函数调用。
 * 
 * @param callee 被调用的表达式，通常是一个函数名或方法名
 * @return Expr 解析得到的函数调用表达式
 */
Expr Parser::finishCall(Expr &callee) {
    // 用于存储函数调用的参数列表
    std::vector<Expr> arguments;
    // 检查当前词法单元是否不是右括号
    if (!check(RIGHT_PAREN)) {
        // 使用 do-while 循环解析参数列表
        do {
            // 检查参数数量是否超过最大限制
            if (arguments.size() >= MAX_PARAMETERS) {
                // 如果超过限制，输出错误信息
                llvm::errs() << peek().getLexeme() << "Can't have more than 255 arguments.";
            }
            // 解析一个参数表达式并添加到参数列表中
            arguments.push_back(expression());
            // 如果当前词法单元是逗号，则继续解析下一个参数
        } while (match(COMMA));
    }
    // 消耗右括号，如果没有则报错
    consume(RIGHT_PAREN, "Expect ')' after arguments.");
    // 创建一个 CallExpr 对象表示函数调用，并返回该对象
    return std::make_unique<CallExpr>(std::move(callee), previous(), std::move(arguments));
}

/**
 * @brief 解析函数调用或属性访问表达式
 * 
 * 该函数从基本表达式开始解析，然后处理连续的函数调用或属性访问操作。
 * 它会不断检查当前词法单元是否为左括号或点号，如果是，则继续解析相应的操作。
 * 
 * @return Expr 解析得到的函数调用或属性访问表达式
 */
Expr Parser::call() {
    // 解析基本表达式
    Expr expr = primary();

    // 使用无限循环处理连续的函数调用或属性访问操作
    while (true) {
        // 检查当前词法单元是否为左括号
        if (match(LEFT_PAREN)) {
            // 如果是左括号，则调用 finishCall 函数完成函数调用的解析
            expr = finishCall(expr);
            // 检查当前词法单元是否为点号
        } else if (match(DOT)) {
            // 消耗属性名标识符，如果没有则报错
            Token name = consume(IDENTIFIER, "Expect property name after '.'.");
            // 创建一个 GetExpr 对象表示属性访问，并更新当前表达式
            expr = std::make_unique<GetExpr>(std::move(expr), name);
            // 如果既不是左括号也不是点号，则跳出循环
        } else {
            break;
        }
    }

    // 返回最终的表达式
    return expr;
}

/**
 * @brief 解析一元表达式，如 ! 或 -
 * 
 * @return Expr 解析得到的一元表达式
 */
Expr Parser::unary() {
    // 检查是否为 ! 或 -
    if (match({BANG, MINUS})) {
        // 获取操作符
        Token operator_ = previous();
        // 递归解析右操作数
        Expr right = unary();
        // 创建一个一元表达式对象
        auto unaryexpr =
            std::make_unique<UnaryExpr>(operator_, static_cast<UnaryOp>(operator_.getType()), std::move(right));
        // 返回该对象
        return std::move(unaryexpr);
    }
    // 如果不是一元表达式，则解析基本表达式
    //return primary();
    return call();
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
    while (match({SLASH, STAR})) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = unary();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(
            std::move(expr), operator_, static_cast<BinaryOp>(operator_.getType()), std::move(right)
        );
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
    while (match({MINUS, PLUS})) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = factor();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(
            std::move(expr), operator_, static_cast<BinaryOp>(operator_.getType()), std::move(right)
        );
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
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = term();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(
            std::move(expr), operator_, static_cast<BinaryOp>(operator_.getType()), std::move(right)
        );
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
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        // 获取操作符
        Token operator_ = previous();
        // 解析右操作数
        Expr right = comparison();
        // 创建一个二元表达式对象
        auto binexpr = std::make_unique<BinaryExpr>(
            std::move(expr), operator_, static_cast<BinaryOp>(operator_.getType()), std::move(right)
        );
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
template<typename T>
bool Parser::match(const std::initializer_list<T> &types) {
    for (auto &type: types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::match(const TokenType type) { return match({type}); }

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
    std::vector<Stmt> statements;
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
    std::vector<Token> parameters;
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
    return std::make_optional<Stmt>(statement());
}

/**
 * @brief 解析表达式语句
 * 
 * 该函数用于解析表达式语句，首先解析表达式，然后消耗分号表示语句结束。
 * 最后创建一个表达式语句对象并返回其智能指针。
 * 
 * @return ExpressionStmtPtr 解析得到的表达式语句的智能指针
 */
ExpressionStmtPtr Parser::expressionStatement() {
    // 解析表达式
    Expr expr = expression();
    // 消耗分号，如果没有则报错
    consume(SEMICOLON, "Expect ';' after expression.");
    // 创建一个表达式语句对象并返回其智能指针
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

/**
 * @brief 解析打印语句
 * 
 * 该函数用于解析打印语句，首先解析要打印的值，然后消耗分号表示语句结束。
 * 最后创建一个打印语句对象并返回其智能指针。
 * 
 * @return PrintStmtPtr 解析得到的打印语句的智能指针
 */
PrintStmtPtr Parser::printStatement() {
    // 解析要打印的值
    Expr value = expression();
    // 消耗分号，如果没有则报错
    consume(SEMICOLON, "Expect ';' after value.");
    // 创建一个打印语句对象并返回其智能指针
    return std::make_shared<PrintStmt>(std::move(value));
}

/**
 * @brief 解析 for 循环语句
 * 
 * 该函数用于解析 for 循环语句，包括初始化语句、循环条件、循环增量和循环体。
 * 它会将 for 循环转换为 while 循环进行处理。
 * 
 * @return Stmt 解析得到的 for 循环语句
 */
Stmt Parser::forStatement() {
    // 消耗左括号，如果没有则报错
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    // 用于存储初始化语句的可选值
    std::optional<Stmt> initializer;
    // 如果当前词法单元是分号，则初始化语句为空
    if (match(SEMICOLON)) {
        initializer = {};
        // 如果当前词法单元是 VAR，则解析变量声明作为初始化语句
    } else if (match(VAR)) {
        initializer = varDeclaration();
        // 否则，解析表达式语句作为初始化语句
    } else {
        initializer = expressionStatement();
    }

    // 用于存储循环条件的可选值，默认为空
    std::optional<Expr> condition = {};
    // 如果当前词法单元不是分号，则解析循环条件
    if (!check(SEMICOLON)) { condition = expression(); }
    // 消耗分号，如果没有则报错
    consume(SEMICOLON, "Expect ';' after loop condition.");

    // 用于存储循环增量的可选值，默认为空
    std::optional<Expr> increment = {};
    // 如果当前词法单元不是右括号，则解析循环增量
    if (!check(RIGHT_PAREN)) { increment = expression(); }
    // 消耗右括号，如果没有则报错
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    // 解析循环体
    Stmt body = statement();

    // 如果有循环增量，则将循环体和循环增量语句组合成一个代码块
    if (increment.has_value()) {
        // 用于存储代码块内的语句
        StmtList statements;
        // 将循环体添加到语句列表中
        statements.push_back(std::move(body));
        // 创建一个表达式语句表示循环增量，并添加到语句列表中
        statements.emplace_back(std::make_shared<ExpressionStmt>(std::move(increment.value())));
        // 创建一个代码块语句对象
        body = std::make_shared<BlockStmt>(std::move(statements));
    }

    // 如果没有循环条件，则使用 true 作为默认循环条件
    if (!condition.has_value()) { condition = std::make_unique<LiteralExpr>(true); }

    // 将循环体和循环条件组合成一个 while 循环语句
    body = std::make_shared<WhileStmt>(std::move(condition.value()), std::move(body));

    // 如果有初始化语句，则将初始化语句和 while 循环语句组合成一个代码块
    if (initializer.has_value()) {
        // 用于存储代码块内的语句
        StmtList blockStatements;
        // 将初始化语句添加到语句列表中
        blockStatements.push_back(std::move(initializer.value()));
        // 将 while 循环语句添加到语句列表中
        blockStatements.push_back(std::move(body));
        // 创建一个代码块语句对象
        body = std::make_shared<BlockStmt>(std::move(blockStatements));
    }

    // 返回最终的语句
    return body;
}

/**
 * @brief 解析 if 语句
 * 
 * 该函数用于解析 if 语句，包括条件表达式、then 分支和可选的 else 分支。
 * 首先消耗左括号，解析条件表达式，再消耗右括号。
 * 接着解析 then 分支语句，若存在 else 关键字，则继续解析 else 分支语句。
 * 最后创建一个 IfStmt 对象并返回其智能指针。
 * 
 * @return IfStmtPtr 解析得到的 if 语句的智能指针
 */
IfStmtPtr Parser::ifStatement() {
    // 消耗左括号，如果没有则报错
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    // 解析条件表达式
    auto condition = expression();
    // 消耗右括号，如果没有则报错
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    // 解析 then 分支语句
    auto thenBranch = statement();
    // 用于存储 else 分支语句的可选值
    std::optional<Stmt> elseBranch;
    // 如果当前词法单元是 ELSE，则解析 else 分支语句
    if (match(ELSE)) { elseBranch = statement(); }
    // 创建一个 IfStmt 对象并返回其智能指针
    return std::make_shared<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

/**
 * @brief 解析 while 语句
 * 
 * 该函数用于解析 while 语句，包括条件表达式和循环体。
 * 首先消耗左括号，解析条件表达式，再消耗右括号。
 * 接着解析循环体语句。
 * 最后创建一个 WhileStmt 对象并返回其智能指针。
 * 
 * @return WhileStmtPtr 解析得到的 while 语句的智能指针
 */
WhileStmtPtr Parser::whileStatement() {
    // 消耗左括号，如果没有则报错
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    // 解析条件表达式
    auto condition = expression();
    // 消耗右括号，如果没有则报错
    consume(RIGHT_PAREN, "Expect ')' after while condition.");
    // 解析循环体语句
    Stmt body = statement();

    // 创建一个 WhileStmt 对象并返回其智能指针
    return std::make_shared<WhileStmt>(std::move(condition), std::move(body));
}


/**
 * @brief 解析返回语句
 * 
 * 该函数用于解析返回语句，首先获取返回关键字对应的词法单元，
 * 然后检查是否有返回值，如果有则解析返回值表达式，
 * 最后消耗分号表示返回语句结束，并创建一个返回语句对象返回。
 * 
 * @return ReturnStmtPtr 解析得到的返回语句的智能指针
 */
ReturnStmtPtr Parser::returnStatement() {
    // 获取返回关键字对应的词法单元
    Token keyword = previous();
    // 用于存储返回值的可选表达式
    std::optional<Expr> value;
    // 如果当前词法单元不是分号，则解析返回值表达式
    if (!check(SEMICOLON)) { value = expression(); }
    // 消耗分号，如果没有则报错
    consume(SEMICOLON, "Expect ';' after return value.");
    // 创建一个返回语句对象并返回其智能指针
    return std::make_shared<ReturnStmt>(keyword, std::move(value));
}
/**
 * @brief 解析通用语句
 * 
 * 该函数用于解析通用语句，根据当前词法单元的类型，尝试解析不同类型的语句。
 * 如果没有匹配到特定类型的语句，则解析为表达式语句。
 * 
 * @return Stmt 解析得到的语句
 */
Stmt Parser::statement() {
    if (match(RETURN)) { return returnStatement(); }
    //如果匹配for,解析for循环
    if (match(FOR)) { return forStatement(); }
    // 如果当前词法单元是 PRINT，则解析打印语句
    if (match(PRINT)) { return printStatement(); }
    // 如果当前词法单元是 RETURN，则解析返回语句
    if (match(RETURN)) { return returnStatement(); }
    // 如果当前词法单元是 WHILE，则解析 while 循环语句
    if (match(WHILE)) { return whileStatement(); }
    // 如果当前词法单元是 FOR，则解析 for 循环语句
    if (match(FOR)) { return forStatement(); }
    // 如果当前词法单元是 IF，则解析 if 条件语句
    if (match(IF)) { return ifStatement(); }
    // 如果当前词法单元是左花括号，则解析代码块语句
    if (match(LEFT_BRACE)) { return std::make_shared<BlockStmt>(block()); }

    // 如果都不匹配，则解析为表达式语句
    return expressionStatement();
}

/**
 * @brief 解析通用语句（重复逻辑，可考虑合并）
 * 
 * 该函数的逻辑与 statement 函数相同，用于解析通用语句。
 * 根据当前词法单元的类型，尝试解析不同类型的语句。
 * 如果没有匹配到特定类型的语句，则解析为表达式语句。
 * 
 * @return Stmt 解析得到的语句
 */
Stmt Parser::statements() {
    // 如果当前词法单元是 PRINT，则解析打印语句
    if (match(PRINT)) { return printStatement(); }
    // 如果当前词法单元是 RETURN，则解析返回语句
    if (match(RETURN)) { return returnStatement(); }
    // 如果当前词法单元是 WHILE，则解析 while 循环语句
    if (match(WHILE)) { return whileStatement(); }
    // 如果当前词法单元是 FOR，则解析 for 循环语句
    if (match(FOR)) { return forStatement(); }
    // 如果当前词法单元是 IF，则解析 if 条件语句
    if (match(IF)) { return ifStatement(); }
    // 如果当前词法单元是左花括号，则解析代码块语句
    if (match(LEFT_BRACE)) { return std::make_shared<BlockStmt>(block()); }

    // 如果都不匹配，则解析为表达式语句
    return expressionStatement();
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
        if (auto decl = declaration(); decl.has_value()) {
            // 如果解析成功，将声明语句添加到程序对象的声明列表中
            program.push_back(std::move(decl.value()));
        }
    }
    // 返回解析得到的程序对象
    return program;
}