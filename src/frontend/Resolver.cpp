
#include "frontend/Resolver.h"
#include "Error/Error.h"
void Resolver::beginScope() { scopes.emplace_back(); }

void Resolver::endScope() { scopes.pop_back(); }

void Resolver::declare(const Token &name) {
    if (scopes.empty()) { return; }
    auto &scope = scopes.back();
    if (scope.contains(name.getLexeme())) { error(name, "Already a variable with this name in this scope."); }
    scope[name.getLexeme()] = false;
}

void Resolver::define(const Token &name) {
    if (scopes.empty()) { return; }
    scopes.back()[name.getLexeme()] = true;
}

void Resolver::resolveLocal(const Assignable &expr, const Token &name) const {
    if (scopes.empty()) { return; }

    for (signed i = static_cast<int>(scopes.size()) - 1; i >= 0; i--) {
        if (scopes.at(i).contains(name.getLexeme())) {
            expr.distance = static_cast<signed>(scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(const FunctionStmtPtr &function, const LoxFunctionType functionType) {
    const LoxFunctionType enclosingFunction = currentFunction;
    currentFunction = functionType;

    beginScope();
    for (auto &param: function->parameters) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();
    currentFunction = enclosingFunction;
}

void Resolver::operator()(const BlockStmtPtr &blockStmt) {
    beginScope();
    resolve(blockStmt->statements);
    endScope();
}

void Resolver::operator()(const FunctionStmtPtr &functionStmt) {
    declare(functionStmt->name);
    define(functionStmt->name);
    resolveFunction(functionStmt, LoxFunctionType::FUNCTION);
}

void Resolver::operator()(const ExpressionStmtPtr &expressionStmt) { resolve(expressionStmt->expression); }

void Resolver::operator()(const PrintStmtPtr &printStmt) { resolve(printStmt->expression); }

void Resolver::operator()(const ReturnStmtPtr &returnStmt) {
    if (currentFunction == LoxFunctionType::NONE) {
        error(returnStmt->keyword, "Can't return from top-level code.");
    } else if (returnStmt->expression.has_value() && currentFunction == LoxFunctionType::INITIALIZER) {
        error(returnStmt->keyword, "Can't return a value from an initializer.");
    }

    resolve(returnStmt->expression);
}

void Resolver::operator()(const VarStmtPtr &varStmt) {
    declare(varStmt->name);
    resolve(varStmt->initializer);
    define(varStmt->name);
}

void Resolver::operator()(const WhileStmtPtr &whileStmt) {
    resolve(whileStmt->condition);
    resolve(whileStmt->body);
}

void Resolver::operator()(const IfStmtPtr &ifStmt) {
    resolve(ifStmt->condition);
    resolve(ifStmt->thenBranch);
    if (ifStmt->elseBranch.has_value()) { resolve(ifStmt->elseBranch.value()); }
}

void Resolver::operator()(const ClassStmtPtr &classStmt) {
    const ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;
    declare(classStmt->name);
    define(classStmt->name);

    if (classStmt->super_class.has_value() &&
        classStmt->name.getLexeme() == classStmt->super_class.value()->name.getLexeme()) {
        error(classStmt->super_class.value()->name, "A class can't inherit from itself.");
    }

    if (classStmt->super_class.has_value()) {
        currentClass = ClassType::SUBCLASS;
        this->operator()(classStmt->super_class.value());
    }

    if (classStmt->super_class.has_value()) {
        beginScope();
        scopes.back()["super"] = true;
    }

    beginScope();
    scopes.back()["this"] = true;

    for (auto &method: classStmt->methods) { resolveFunction(method, method->type); }

    endScope();

    if (classStmt->super_class.has_value()) { endScope(); }

    currentClass = enclosingClass;
}

void Resolver::operator()(const AssignExprPtr &assignExpr) {
    resolve(assignExpr->value);
    resolveLocal(*assignExpr, assignExpr->name);
}

void Resolver::operator()(const BinaryExprPtr &binaryExpr) {
    resolve(binaryExpr->left);
    resolve(binaryExpr->right);
}

void Resolver::operator()(const CallExprPtr &callExpr) {
    resolve(callExpr->callee);
    for (auto &arg: callExpr->arguments) { resolve(arg); }
}

void Resolver::operator()(const GetExprPtr &getExpr) { resolve(getExpr->object); }

void Resolver::operator()(const SetExprPtr &setExpr) {
    resolve(setExpr->object);
    resolve(setExpr->value);
}

void Resolver::operator()(const ThisExprPtr &thisExpr) const {
    if (currentClass == ClassType::NONE) {
        error(thisExpr->name, "Can't use 'this' outside of a class.");
        return;
    }

    resolveLocal(*thisExpr, thisExpr->name);
}

void Resolver::operator()(const SuperExprPtr &superExpr) const {
    if (currentClass == ClassType::NONE) {
        error(superExpr->name, "Can't use 'super' outside of a class.");
    } else if (currentClass != ClassType::SUBCLASS) {
        error(superExpr->name, "Can't use 'super' in a class with no superclass.");
    }
    resolveLocal(*superExpr, superExpr->name);
}

void Resolver::operator()(const VarExprPtr &varExpr) {
    if (!scopes.empty() && scopes.back().contains(varExpr->name.getLexeme()) &&
        !scopes.back()[varExpr->name.getLexeme()]) {
        error(varExpr->name, "Can't read local variable in its own initializer.");
        return;
    }

    resolveLocal(*varExpr, varExpr->name);
}

void Resolver::operator()(const GroupingExprPtr &groupingExpr) { resolve(groupingExpr->expression); }

void Resolver::operator()(const LiteralExprPtr &) const {}

void Resolver::operator()(const LogicalExprPtr &logicalExpr) {
    resolve(logicalExpr->left);
    resolve(logicalExpr->right);
}

void Resolver::operator()(const UnaryExprPtr &unaryExpr) { resolve(unaryExpr->expression); }


void Resolver::resolve(const std::optional<Expr> &opt) {
    if (opt.has_value()) { resolve(opt.value());
}
}

void Resolver::resolve(const Expr &expr) { std::visit(*this, expr); }

void Resolver::resolve(const Stmt &stmt) { std::visit(*this, stmt); }


void Resolver::resolve(const Program &program) {
    for (auto &item: program) { resolve(item); }
}
