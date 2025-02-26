
#include "frontend/Resolver.h"
#include "Error/Error.h"
/**
 * @brief 开始一个新的作用域
 * 
 * 该函数向作用域栈 `scopes` 中添加一个新的空作用域，用于后续变量的声明和定义。
 */
void Resolver::beginScope() { 
    // 向作用域栈中添加一个新的空作用域
    scopes.emplace_back(); 
}

/**
 * @brief 结束当前作用域
 * 
 * 该函数从作用域栈 `scopes` 中移除最后一个作用域，即当前作用域，标志着该作用域的结束。
 */
void Resolver::endScope() { 
    // 从作用域栈中移除最后一个作用域
    scopes.pop_back(); 
}

/**
 * @brief 声明一个变量
 * 
 * 该函数在当前作用域中声明一个变量。如果当前作用域中已经存在同名变量，则会抛出错误。
 * 声明变量时，会将该变量标记为未定义状态（`false`）。
 * 
 * @param name 变量的 Token 对象
 */
void Resolver::declare(const Token &name) {
    // 如果作用域栈为空，直接返回
    if (scopes.empty()) { return; }
    // 获取当前作用域
    auto &scope = scopes.back();
    // 检查当前作用域中是否已经存在同名变量
    if (scope.contains(name.getLexeme())) { 
        // 如果存在，抛出错误
        error(name, "Already a variable with this name in this scope."); 
    }
    // 声明变量，标记为未定义状态
    scope[name.getLexeme()] = false;
}

/**
 * @brief 定义一个变量
 * 
 * 该函数在当前作用域中定义一个变量，即将该变量标记为已定义状态（`true`）。
 * 如果作用域栈为空，则不进行任何操作。
 * 
 * @param name 变量的 Token 对象
 */
void Resolver::define(const Token &name) {
    // 如果作用域栈为空，直接返回
    if (scopes.empty()) { return; }
    // 在当前作用域中定义变量，标记为已定义状态
    scopes.back()[name.getLexeme()] = true;
}

/**
 * @brief 解析局部变量的作用域距离
 * 
 * 该函数用于确定局部变量在作用域栈中的距离。从当前作用域开始，逐层向上查找，
 * 直到找到同名变量或到达作用域栈的顶部。如果找到变量，则将其作用域距离赋值给 `expr.distance`。
 * 
 * @param expr 可赋值表达式对象，包含变量的作用域距离信息
 * @param name 变量的 Token 对象
 */
void Resolver::resolveLocal(const Assignable &expr, const Token &name) const {
    // 如果作用域栈为空，直接返回
    if (scopes.empty()) { return; }

    // 从当前作用域开始，逐层向上查找变量
    for (signed i = static_cast<int>(scopes.size()) - 1; i >= 0; i--) {
        // 检查当前作用域中是否包含同名变量
        if (scopes.at(i).contains(name.getLexeme())) {
            // 计算变量的作用域距离
            expr.distance = static_cast<signed>(scopes.size() - 1 - i);
            return;
        }
    }
}

/**
 * @brief 解析函数声明
 * 
 * 该函数用于解析函数声明，包括参数和函数体。在解析函数时，会创建一个新的作用域，
 * 并将函数的参数声明和定义在该作用域中。然后递归解析函数体，最后结束该作用域。
 * 
 * @param function 函数声明语句的智能指针
 * @param functionType 函数的类型
 */
void Resolver::resolveFunction(const FunctionStmtPtr &function, const LoxFunctionType functionType) {
    // 保存当前函数类型
    const LoxFunctionType enclosingFunction = currentFunction;
    // 设置当前函数类型
    currentFunction = functionType;

    // 开始一个新的作用域
    beginScope();
    // 遍历函数的参数
    for (auto &param: function->parameters) {
        // 声明参数
        declare(param);
        // 定义参数
        define(param);
    }
    // 解析函数体
    resolve(function->body);
    // 结束当前作用域
    endScope();
    // 恢复之前的函数类型
    currentFunction = enclosingFunction;
}

/**
 * @brief 处理代码块语句
 * 
 * 该函数用于处理代码块语句，包括开始一个新的作用域，解析代码块中的语句，最后结束该作用域。
 * 
 * @param blockStmt 代码块语句的智能指针
 */
void Resolver::operator()(const BlockStmtPtr &blockStmt) {
    // 开始一个新的作用域
    beginScope();
    // 解析代码块中的语句
    resolve(blockStmt->statements);
    // 结束当前作用域
    endScope();
}

/**
 * @brief 处理函数声明语句
 * 
 * 该函数用于处理函数声明语句，包括声明函数名、定义函数名，并解析函数体。
 * 
 * @param functionStmt 函数声明语句的智能指针
 */
void Resolver::operator()(const FunctionStmtPtr &functionStmt) {
    // 声明函数名
    declare(functionStmt->name);
    // 定义函数名
    define(functionStmt->name);
    // 解析函数体
    resolveFunction(functionStmt, LoxFunctionType::FUNCTION);
}

/**
 * @brief 处理表达式语句
 * 
 * 该函数用于处理表达式语句，即解析表达式语句中的表达式。
 * 
 * @param expressionStmt 表达式语句的智能指针
 */
void Resolver::operator()(const ExpressionStmtPtr &expressionStmt) { 
    // 解析表达式语句中的表达式
    resolve(expressionStmt->expression); 
}

/**
 * @brief 处理打印语句
 * 
 * 该函数用于处理打印语句，即解析打印语句中的表达式。
 * 
 * @param printStmt 打印语句的智能指针
 */
void Resolver::operator()(const PrintStmtPtr &printStmt) { 
    // 解析打印语句中的表达式
    resolve(printStmt->expression); 
}

/**
 * @brief 处理返回语句
 * 
 * 该函数用于处理返回语句，检查返回语句是否合法，并解析返回语句中的表达式。
 * 
 * @param returnStmt 返回语句的智能指针
 */
void Resolver::operator()(const ReturnStmtPtr &returnStmt) {
    // 检查是否在顶级代码中使用返回语句
    if (currentFunction == LoxFunctionType::NONE) {
        // 如果是，抛出错误
        error(returnStmt->keyword, "Can't return from top-level code.");
    } 
    // 检查是否在构造函数中返回值
    else if (returnStmt->expression.has_value() && currentFunction == LoxFunctionType::INITIALIZER) {
        // 如果是，抛出错误
        error(returnStmt->keyword, "Can't return a value from an initializer.");
    }

    // 解析返回语句中的表达式
    resolve(returnStmt->expression);
}

/**
 * @brief 处理变量声明语句
 * 
 * 该函数用于处理变量声明语句，包括声明变量、解析变量的初始化表达式，并定义变量。
 * 
 * @param varStmt 变量声明语句的智能指针
 */
void Resolver::operator()(const VarStmtPtr &varStmt) {
    // 声明变量
    declare(varStmt->name);
    // 解析变量的初始化表达式
    resolve(varStmt->initializer);
    // 定义变量
    define(varStmt->name);
}

/**
 * @brief 处理 while 循环语句
 * 
 * 该函数用于处理 while 循环语句，包括解析循环条件和循环体。
 * 
 * @param whileStmt while 循环语句的智能指针
 */
void Resolver::operator()(const WhileStmtPtr &whileStmt) {
    // 解析循环条件
    resolve(whileStmt->condition);
    // 解析循环体
    resolve(whileStmt->body);
}

/**
 * @brief 处理 if 条件语句
 * 
 * 该函数用于处理 if 条件语句，包括解析条件表达式、真分支语句和可选的假分支语句。
 * 
 * @param ifStmt if 条件语句的智能指针
 */
void Resolver::operator()(const IfStmtPtr &ifStmt) {
    // 解析条件表达式
    resolve(ifStmt->condition);
    // 解析真分支语句
    resolve(ifStmt->thenBranch);
    // 检查是否有假分支语句
    if (ifStmt->elseBranch.has_value()) { 
        // 如果有，解析假分支语句
        resolve(ifStmt->elseBranch.value()); 
    }
}

/**
 * @brief 处理类声明语句
 * 
 * 该函数用于处理类声明语句，包括声明类名、定义类名、检查继承关系、解析类的方法等。
 * 
 * @param classStmt 类声明语句的智能指针
 */
void Resolver::operator()(const ClassStmtPtr &classStmt) {
    // 保存当前类的类型
    const ClassType enclosingClass = currentClass;
    // 设置当前类的类型为 CLASS
    currentClass = ClassType::CLASS;
    // 声明类名
    declare(classStmt->name);
    // 定义类名
    define(classStmt->name);

    // 检查类是否继承自自身
    if (classStmt->super_class.has_value() &&
        classStmt->name.getLexeme() == classStmt->super_class.value()->name.getLexeme()) {
        // 如果是，抛出错误
        error(classStmt->super_class.value()->name, "A class can't inherit from itself.");
    }

    // 检查类是否有父类
    if (classStmt->super_class.has_value()) {
        // 如果有，设置当前类的类型为 SUBCLASS
        currentClass = ClassType::SUBCLASS;
        // 解析父类
        this->operator()(classStmt->super_class.value());
    }

    // 检查类是否有父类
    if (classStmt->super_class.has_value()) {
        // 如果有，开始一个新的作用域
        beginScope();
        // 在新作用域中定义 super 变量
        scopes.back()["super"] = true;
    }

    // 开始一个新的作用域
    beginScope();
    // 在新作用域中定义 this 变量
    scopes.back()["this"] = true;

    // 遍历类的方法
    for (auto &method: classStmt->methods) {
        // 判断方法是否为构造函数
        const LoxFunctionType methodType = method->name.getLexeme() == "init"
            ? LoxFunctionType::INITIALIZER
            : LoxFunctionType::METHOD;
        // 解析方法
        resolveFunction(method, methodType);
    }

    // 结束当前作用域
    endScope();
    // 检查类是否有父类
    if (classStmt->super_class.has_value()) { 
        // 如果有，结束父类作用域
        endScope(); 
    }

    // 恢复之前的类的类型
    currentClass = enclosingClass;
}

/**
 * @brief 处理赋值表达式
 * 
 * 该函数用于处理赋值表达式，先解析赋值表达式右侧的值，再解析赋值目标变量的作用域。
 * 
 * @param assignExpr 赋值表达式的智能指针
 */
void Resolver::operator()(const AssignExprPtr &assignExpr) {
    // 解析赋值表达式右侧的值
    resolve(assignExpr->value);
    // 解析赋值目标变量的作用域
    resolveLocal(*assignExpr, assignExpr->name);
}

/**
 * @brief 处理二元表达式
 * 
 * 该函数用于处理二元表达式，分别解析二元表达式的左操作数和右操作数。
 * 
 * @param binaryExpr 二元表达式的智能指针
 */
void Resolver::operator()(const BinaryExprPtr &binaryExpr) {
    // 解析二元表达式的左操作数
    resolve(binaryExpr->left);
    // 解析二元表达式的右操作数
    resolve(binaryExpr->right);
}

/**
 * @brief 处理函数调用表达式
 * 
 * 该函数用于处理函数调用表达式，先解析被调用的函数，再解析函数调用的参数。
 * 
 * @param callExpr 函数调用表达式的智能指针
 */
void Resolver::operator()(const CallExprPtr &callExpr) {
    // 解析被调用的函数
    resolve(callExpr->callee);
    // 遍历函数调用的参数
    for (auto &arg: callExpr->arguments) { 
        // 解析每个参数
        resolve(arg); 
    }
}

/**
 * @brief 处理属性获取表达式
 * 
 * 该函数用于处理属性获取表达式，解析属性所属的对象。
 * 
 * @param getExpr 属性获取表达式的智能指针
 */
void Resolver::operator()(const GetExprPtr &getExpr) { 
    // 解析属性所属的对象
    resolve(getExpr->object); 
}

/**
 * @brief 处理属性设置表达式
 * 
 * 该函数用于处理属性设置表达式，先解析属性所属的对象，再解析要设置的值。
 * 
 * @param setExpr 属性设置表达式的智能指针
 */
void Resolver::operator()(const SetExprPtr &setExpr) {
    // 解析属性所属的对象
    resolve(setExpr->object);
    // 解析要设置的值
    resolve(setExpr->value);
}

/**
 * @brief 处理 this 表达式
 * 
 * 该函数用于处理 this 表达式，检查是否在类的内部使用 this，如果不在类内部则抛出错误，
 * 否则解析 this 的作用域。
 * 
 * @param thisExpr this 表达式的智能指针
 */
void Resolver::operator()(const ThisExprPtr &thisExpr) const {
    // 检查是否在类的内部使用 this
    if (currentClass == ClassType::NONE) {
        // 如果不在类内部，抛出错误
        error(thisExpr->name, "Can't use 'this' outside of a class.");
        return;
    }
    // 解析 this 的作用域
    resolveLocal(*thisExpr, thisExpr->name);
}

/**
 * @brief 处理 super 表达式
 * 
 * 该函数用于处理 super 表达式，检查是否在类的内部使用 super，以及该类是否有父类，
 * 如果不满足条件则抛出错误，否则解析 super 的作用域。
 * 
 * @param superExpr super 表达式的智能指针
 */
void Resolver::operator()(const SuperExprPtr &superExpr) const {
    // 检查是否在类的内部使用 super
    if (currentClass == ClassType::NONE) {
        // 如果不在类内部，抛出错误
        error(superExpr->name, "Can't use 'super' outside of a class.");
    } 
    // 检查该类是否有父类
    else if (currentClass != ClassType::SUBCLASS) {
        // 如果没有父类，抛出错误
        error(superExpr->name, "Can't use 'super' in a class with no superclass.");
    }
    // 解析 super 的作用域
    resolveLocal(*superExpr, superExpr->name);
}

/**
 * @brief 处理变量表达式
 * 
 * 该函数用于处理变量表达式，检查变量是否在其自身初始化器中被读取，如果是则抛出错误，
 * 否则解析变量的作用域。
 * 
 * @param varExpr 变量表达式的智能指针
 */
void Resolver::operator()(const VarExprPtr &varExpr) {
    // 检查作用域栈是否不为空，当前作用域是否包含该变量，以及该变量是否未定义
    if (!scopes.empty() && scopes.back().contains(varExpr->name.getLexeme()) &&
        !scopes.back()[varExpr->name.getLexeme()]) {
        // 如果是，则抛出错误
        error(varExpr->name, "Can't read local variable in its own initializer.");
        return;
    }
    // 解析变量的作用域
    resolveLocal(*varExpr, varExpr->name);
}

/**
 * @brief 处理分组表达式
 * 
 * 该函数用于处理分组表达式，即解析分组表达式中的表达式。
 * 
 * @param groupingExpr 分组表达式的智能指针
 */
void Resolver::operator()(const GroupingExprPtr &groupingExpr) { 
    // 解析分组表达式中的表达式
    resolve(groupingExpr->expression); 
}

/**
 * @brief 处理字面量表达式
 * 
 * 该函数用于处理字面量表达式，由于字面量表达式不需要额外的解析，所以此函数为空。
 * 
 * @param 字面量表达式的智能指针，这里未命名参数，因为在函数中未使用
 */
void Resolver::operator()(const LiteralExprPtr &) const {}

/**
 * @brief 处理逻辑表达式
 * 
 * 该函数用于处理逻辑表达式，分别解析逻辑表达式的左操作数和右操作数。
 * 
 * @param logicalExpr 逻辑表达式的智能指针
 */
void Resolver::operator()(const LogicalExprPtr &logicalExpr) {
    // 解析逻辑表达式的左操作数
    resolve(logicalExpr->left);
    // 解析逻辑表达式的右操作数
    resolve(logicalExpr->right);
}

/**
 * @brief 处理一元表达式
 * 
 * 该函数用于处理一元表达式，即解析一元表达式中的表达式。
 * 
 * @param unaryExpr 一元表达式的智能指针
 */
void Resolver::operator()(const UnaryExprPtr &unaryExpr) { 
    // 解析一元表达式中的表达式
    resolve(unaryExpr->expression); 
}

/**
 * @brief 解析可选表达式
 * 
 * 该函数用于解析可选表达式，如果可选表达式有值，则解析该表达式。
 * 
 * @param opt 可选表达式
 */
void Resolver::resolve(const std::optional<Expr> &opt) {
    // 检查可选表达式是否有值
    if (opt.has_value()) { 
        // 如果有值，则解析该表达式
        resolve(opt.value()); 
    }
}

/**
 * @brief 解析表达式
 * 
 * 该函数使用 std::visit 对表达式进行多态调用，根据表达式的具体类型调用相应的处理函数。
 * 
 * @param expr 表达式
 */
void Resolver::resolve(const Expr &expr) { 
    // 使用 std::visit 对表达式进行多态调用
    std::visit(*this, expr); 
}

/**
 * @brief 解析语句
 * 
 * 该函数使用 std::visit 对语句进行多态调用，根据语句的具体类型调用相应的处理函数。
 * 
 * @param stmt 语句
 */
void Resolver::resolve(const Stmt &stmt) { 
    // 使用 std::visit 对语句进行多态调用
    std::visit(*this, stmt); 
}

/**
 * @brief 解析程序
 * 
 * 该函数遍历程序中的每个语句或表达式，并调用 resolve 函数进行解析。
 * 
 * @param program 程序，包含多个语句或表达式
 */
void Resolver::resolve(const Program &program) {
    // 遍历程序中的每个语句或表达式
    for (const auto &item: program) { 
        // 解析每个语句或表达式
        resolve(item); 
    }
}
