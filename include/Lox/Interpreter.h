#pragma once

#include "Lox/Environment.h"
#include "Lox/LoxObject.h"
#include "frontend/Ast.h"
#include <memory>

#define MAX_CALL_DEPTH 100
struct Return {
    LoxObject &value;
    ~Return() = default;
    explicit Return(LoxObject &value) : value(value) {};
};

struct Nothing {};
using StmtResult = std::variant<LoxObject, Return, Nothing>;
class Interpreter {
public:
    Interpreter();
    ~Interpreter();
    //void interpret(const std::vector<StmtPtr> &statements);
    StmtResult operator()(const ExpressionStmtPtr &expressionStmt);
    StmtResult operator()(const IfStmtPtr &ifStmtPtr);
    StmtResult operator()(const PrintStmtPtr &printStmt);
    StmtResult operator()(const VarStmtPtr &varStmt);
    StmtResult operator()(const FunctionStmtPtr &functionStmt);
    StmtResult operator()(const ReturnStmtPtr &returnStmt);
    StmtResult operator()(const BlockStmtPtr &blockStmt);
    StmtResult operator()(const WhileStmtPtr &whileStmt);
    StmtResult operator()(const ClassStmtPtr &classStmt);
    LoxObject operator()(const BinaryExprPtr &binaryExpr);
    LoxObject operator()(const CallExprPtr &callExpr);
    LoxObject operator()(const GetExprPtr &getExpr);
    LoxObject operator()(const SetExprPtr &setExpr);
    LoxObject operator()(const ThisExprPtr &thisExpr) const;
    LoxObject operator()(const SuperExprPtr &superExpr) const;
    LoxObject operator()(const GroupingExprPtr &groupingExpr);
    LoxObject operator()(const LiteralExprPtr &literalExpr) const;
    LoxObject operator()(const LogicalExprPtr &logicalExpr);
    LoxObject operator()(const UnaryExprPtr &unaryExpr);
    LoxObject operator()(const VarExprPtr &varExpr) const;
    LoxObject operator()(const AssignExprPtr &assignExpr);

    LoxObject evaluate(const Expr &expr);
    StmtResult evaluate(const Stmt &stmt);
    void evaluate(const Program &program);
    StmtResult executeblock(const StmtList &statements, const EnvironmentPtr &newenvironment);

private:
    EnvironmentPtr globals = std::make_shared<Environment>();
    EnvironmentPtr environment = globals;
    int function_depth = 0;
};