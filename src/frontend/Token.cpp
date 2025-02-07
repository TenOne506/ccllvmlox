// 引入标准库中的string类，用于处理字符串
#include "frontend/Token.h"
#include <llvm/ADT/StringRef.h>
#include <string>


// 转换 literal 为字符串
static std::string literal_to_string(const Literal &literal) {
    std::ostringstream oss;
    // Use std::visit to handle different types in the variant
    std::visit(
        [&oss](const auto &val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::nullptr_t>) {
                oss << "nullptr";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string_view>) {
                oss << val;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
                oss << val;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::monostate>) {
                oss << "empty";// Special handling for std::monostate
            }
        },
        literal
    );

    return oss.str();
}
// 转换 std::optional<Literal> 为字符串
static std::string optional_literal_to_string(const std::optional<Literal> &opt_literal) {
    if (opt_literal) {
        return literal_to_string(*opt_literal);// 如果有值，转换为字符串
    }
    return "no value";// 如果没有值，返回默认字符串
}
/**
     * @brief 将Token对象转换为字符串表示。
     * 
     * @return std::string 包含词法单元类型、词素和字面量的字符串。
     */
[[nodiscard]] std::string Token::toString() const {
    return std::to_string(static_cast<int>(type)) + " " + lexeme.str() + " ";
}