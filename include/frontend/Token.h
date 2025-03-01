// 引入标准库中的string类，用于处理字符串
#pragma once
#include <llvm/ADT/StringRef.h>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
/**
 * @enum TokenType
 * @brief 定义了Lox语言中所有可能的词法单元类型。
 * 
 * 该枚举类型包含了各种单字符、双字符、字面量和关键字的词法单元类型，
 * 以及表示文件结束的特殊类型。
 */
enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, // 左括号 '('
    RIGHT_PAREN,// 右括号 ')'
    LEFT_BRACE, // 左大括号 '{'
    RIGHT_BRACE,// 右大括号 '}'
    COMMA,      // 逗号 ','
    DOT,        // 点 '.'
    MINUS,      // 减号 '-'
    PLUS,       // 加号 '+'
    SEMICOLON,  // 分号 ';'
    SLASH,      // 斜杠 '/'
    STAR,       // 星号 '*'

    // One or two character tokens.
    BANG,         // 感叹号 '!'
    BANG_EQUAL,   // 不等于 '!='
    EQUAL,        // 等号 '='
    EQUAL_EQUAL,  // 等于 '=='
    GREATER,      // 大于 '>'
    GREATER_EQUAL,// 大于等于 '>='
    LESS,         // 小于 '<'
    LESS_EQUAL,   // 小于等于 '<='

    // Literals.
    IDENTIFIER,// 标识符，如变量名、函数名等
    STRING,    // 字符串字面量
    NUMBER,    // 数字字面量

    // Keywords.
    AND,   // 逻辑与 'and'
    CLASS, // 类定义关键字 'class'
    ELSE,  // 条件语句中的 'else'
    FALSE, // 布尔值 'false'
    FUN,   // 函数定义关键字 'fun'
    FOR,   // 循环语句 'for'
    IF,    // 条件语句 'if'
    NIL,   // 空值 'nil'
    OR,    // 逻辑或 'or'
    PRINT, // 打印语句 'print'
    RETURN,// 返回语句 'return'
    SUPER, // 超类引用 'super'
    THIS,  // 当前对象引用 'this'
    TRUE,  // 布尔值 'true'
    VAR,   // 变量声明关键字 'var'
    WHILE, // 循环语句 'while'

    LoxEOF// 文件结束符
};
using Literal = std::variant<std::nullptr_t, std::string_view, double, bool>;
/**
 * @class Token
 * @brief 表示一个词法单元，包含类型、词素、字面量和行号信息。
 * 
 * 该类封装了词法单元的基本属性，并提供了一个方法用于将其转换为字符串表示。
 */
class Token {
private:
    // 成员变量
    TokenType type;                // 词法单元的类型
    std::string lexeme;            // 词法单元的词素，即源代码中的实际字符序列
    Literal literal;// 在C++中，通常需要具体指定类型，这里假设literal为std::string
    unsigned int line;             // 词法单元所在的行号

public:
    /**
     * @brief 构造一个新的Token对象。
     * 
     * @param type 词法单元的类型。
     * @param lexeme 词法单元的词素。
     * @param literal 词法单元的字面量值。
     * @param line 词法单元所在的行号。
     */
    explicit Token(const TokenType type, const std::string_view lexeme, const Literal &literal, const unsigned int line)
        : type{type}, lexeme{lexeme}, literal{literal}, line{line} {}
    [[nodiscard]] TokenType getType() const { return type; }
    // 转换 literal 为字符串
    static std::string literal_to_string(const Literal &literal);
    // 转换 std::optional<Literal> 为字符串
    static std::string optional_literal_to_string(const std::optional<Literal> &opt_literal);

    /**
     * @brief 将Token对象转换为字符串表示。
     * 
     * 该方法将词法单元的类型、词素和字面量信息组合成一个字符串，方便调试和输出。
     * 
     * @return std::string 包含词法单元类型、词素和字面量的字符串。
     */
    [[nodiscard]] std::string toString() const;

    /**
     * @brief 获取词法单元的字面量值。
     * 
     * 该方法返回词法单元的字面量值，如果字面量存在则返回其值，否则可能会抛出异常。
     * 
     * @return Literal 词法单元的字面量值。
     */
    [[nodiscard]] Literal getLiteral() const { return literal; }

    /**
     * @brief 获取词法单元所在的行号。
     * 
     * 该方法返回词法单元在源代码中出现的行号，用于定位错误和调试。
     * 
     * @return int 词法单元所在的行号。
     */
    [[nodiscard]] unsigned int getLine() const { return line; }

    /**
     * @brief 获取词法单元的词素。
     * 
     * 该方法返回词法单元在源代码中的实际字符序列。
     * 
     * @return std::string 词法单元的词素。
     */
    [[nodiscard]] std::string_view getLexeme() const { return lexeme; }
   
};