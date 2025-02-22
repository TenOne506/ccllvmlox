#include "Lox/LoxInstance.h"
#include "Lox/LoxObject.h"
#include <Lox/LoxFunction.h>
#include <string>

LoxObject LoxInstance::get(const Token &name) {
    if (fields.contains(name.getLexeme())) { return fields[name.getLexeme()]; }

    if (const auto method = klass->findMethod(name.getLexeme()); method != nullptr) {
        const auto instance = shared_from_this();
        return std::reinterpret_pointer_cast<LoxCallable>(method->bind(instance));
    }

    throw runtime_error(name, "Undefined property '" + std::string(name.getLexeme()) + "'.");
}

void LoxInstance::set(const Token &name, const LoxObject &value) { fields[name.getLexeme()] = value; }

std::string LoxInstance::to_string() const { return (std::string(this->klass->name) +"instance"); }