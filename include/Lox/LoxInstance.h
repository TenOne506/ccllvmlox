#pragma once
#include "Lox/LoxClass.h"
#include "Lox/LoxObject.h"
#include <memory>
#include <unordered_map>
#include <utility>

class LoxInstance : std::enable_shared_from_this<LoxInstance> {
public:
    LoxClassPtr klass;
    std::unordered_map<std::string_view, LoxObject> fields;

    explicit LoxInstance(LoxClassPtr klass) : klass{std::move(klass)} {}

    LoxObject get(const Token &name);
    void set(const Token &name, const LoxObject &value);
    [[nodiscard]] std::string to_string() const;
};