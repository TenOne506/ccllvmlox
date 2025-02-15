#pragma once

#include "Error/Error.h"
#include <memory>


// Lox runtime types.
using LoxNil = std::nullptr_t;
using LoxString = std::string;
using LoxNumber = double;
using LoxBoolean = bool;
class LoxCallable;
class LoxFunction;
class LoxClass;
class LoxInstance;
using LoxCallablePtr = std::shared_ptr<LoxCallable>;
using LoxFunctionPtr = std::shared_ptr<LoxFunction>;
using LoxInstancePtr = std::shared_ptr<LoxInstance>;
using LoxClassPtr = std::shared_ptr<LoxClass>;
using LoxObject = std::variant<LoxNil, LoxString, LoxNumber, LoxBoolean, LoxCallablePtr, LoxInstancePtr>;

bool isTruthy(const LoxObject &object);
std::string to_string(const LoxObject &object);