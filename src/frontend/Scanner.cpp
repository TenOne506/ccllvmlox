#include "frontend/Scanner.h"
#include "Error/Error.h"
#include "frontend/Token.h"
#include <llvm/ADT/StringRef.h>
#include <optional>


std::unordered_map<std::string, TokenType> Scanner::keywords = {
    {"and", AND},   {"class", CLASS}, {"else", ELSE}, {"false", FALSE}, {"for", FOR},       {"fun", FUN},
    {"if", IF},     {"nil", NIL},     {"or", OR},     {"print", PRINT}, {"return", RETURN}, {"super", SUPER},
    {"this", THIS}, {"true", TRUE},   {"var", VAR},   {"while", WHILE}
};

/**
 * @brief 前进到源代码的下一个字符并返回该字符。
 * 
 * 此函数将当前扫描位置 `current` 向前移动一位，并返回当前位置的字符。
 * 如果已经到达源代码的末尾，将标记错误并返回空字符 '\0'。
 * 如果遇到换行符，会增加行号。
 * 
 * @return 当前位置的字符，如果到达末尾则返回 '\0'。
 */
char Scanner::advance() {
    // 直接返回当前字符并将 current 指针后移
    return source[current++];
}

/**
 * @brief 向词法单元列表中添加一个无字面量的词法单元。
 * 
 * 此函数调用重载的 `addToken` 函数，传入空的字面量。
 * 
 * @param type 要添加的词法单元的类型。
 */
void Scanner::addToken(TokenType type) {
    // 调用重载的 addToken 函数，传入空的字面量
    addToken(type, "");
}

/**
 * @brief 向词法单元列表中添加一个带有字面量的词法单元。
 * 
 * 此函数根据当前扫描的起始位置 `start` 和当前位置 `current` 提取源代码中的文本，
 * 并创建一个新的词法单元添加到词法单元列表 `tokens` 中。
 * 
 * @param type 要添加的词法单元的类型。
 * @param literal 词法单元的字面量。
 */
// void Scanner::addToken(TokenType type, std::string literal) {
//     // 从源代码中提取当前扫描的文本
//     std::string text = source.substr(start, current - start);
//     // 创建一个新的词法单元并添加到词法单元列表中
//     tokens.emplace_back(type, text, literal, line);
// }
void Scanner::addToken(TokenType type, Literal literal) {
    auto lexeme = std::string_view(source).substr(start, current - start);
    tokens.emplace_back(type, lexeme, literal, line);
}
/**
 * @brief 检查当前字符是否与预期字符匹配，如果匹配则前进到下一个字符。
 * 
 * 该函数首先检查扫描器是否已经到达源代码的末尾，如果是则返回 false。
 * 接着检查当前字符是否与预期字符相等，如果不相等则返回 false。
 * 如果当前字符与预期字符相等，则将扫描位置 `current` 前进一位，并返回 true。
 * 
 * @param expected 期望匹配的字符。
 * @return 如果当前字符与预期字符匹配且成功前进到下一个字符，返回 true；否则返回 false。
 */
bool Scanner::match(char expected) {
    // 检查是否已经到达源代码的末尾，如果是则返回 false
    if (isAtEnd()) { return false; }
    // 检查当前字符是否与预期字符不相等，如果不相等则返回 false
    if (source[current] != expected) { return false; }
    // 如果匹配成功，将当前位置指针向后移动一位
    current++;
    // 返回匹配成功的标志
    return true;
}

/**
 * @brief 检查扫描器是否已经到达源代码的末尾。
 * 
 * 该函数通过比较当前扫描位置 `current` 和源代码的总长度 `source.size()` 来判断是否到达末尾。
 * 
 * @return 如果 `current` 大于或等于 `source.size()`，则返回 `true`，表示已经到达末尾；否则返回 `false`。
 */
[[nodiscard]] bool Scanner::isAtEnd() const {
    // 检查当前位置是否超出了源代码的长度
    return current >= source.length();
}

/**
 * @brief 查看当前位置的字符，但不移动扫描位置。
 * 
 * 如果扫描器已经到达源代码的末尾，则返回空字符 '\0'；
 * 否则，返回当前位置的字符。
 * 
 * @return 当前位置的字符或空字符 '\0'。
 */
char Scanner::peek() {
    // 检查是否到达源代码末尾
    if (isAtEnd()) { return '\0'; }
    // 返回当前位置的字符
    return source[current];
}

/**
 * @brief 查看当前位置的下一个字符，但不移动扫描位置。
 * 
 * 如果下一个位置超出了源代码的范围，则返回空字符 '\0'；
 * 否则，返回下一个位置的字符。
 * 
 * @return 下一个位置的字符或空字符 '\0'。
 */
char Scanner::peekNext() {
    // 检查下一个位置是否超出源代码范围
    if (current + 1 >= source.size()) { return '\0'; }
    // 返回下一个位置的字符
    return source[current + 1];
}

/**
 * @brief 处理 Lox 语言中的字符串字面量。
 * 
 * 该函数会持续扫描，直到遇到字符串结束符 '"' 或到达源代码末尾。
 * 如果在扫描过程中遇到换行符，则更新行号。
 * 如果到达末尾仍未找到字符串结束符，则报告错误。
 * 最后，提取字符串内容并添加到词法单元列表中。
 */
void Scanner::loxstring() {
    // 持续扫描，直到遇到字符串结束符 '"' 或到达源代码末尾
    while (peek() != '"' && !isAtEnd()) {
        // 如果遇到换行符，更新行号
        if (peek() == '\n') { line++; }
        // 移动到下一个字符
        advance();
    }
    // 如果到达末尾仍未找到字符串结束符，报告错误
    if (isAtEnd()) {
        error(line, "Unterminated string.");
        return;
    }
    // 移动到字符串结束符之后
    advance();
    // // 提取字符串内容
    //Bug在这里，生命周期的坑，这里的字符串是临时的，不能用引用
    //std::string text = source.substr(start + 1, current - start - 2);
    // // 添加字符串词法单元
    //addToken(STRING, text);
    addToken(STRING, std::string_view(source).substr(start + 1, current - 1 - start - 1));
}

/**
 * @brief 检查字符是否为数字。
 * 
 * 该函数检查给定的字符是否在 '0' 到 '9' 的范围内。
 * 
 * @param c 要检查的字符。
 * @return 如果字符是数字，则返回 true；否则返回 false。
 */
bool Scanner::isDigit(char c) {
    // 检查字符是否在 '0' 到 '9' 的范围内
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

bool Scanner::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Scanner::identifier() {
    while (isAlphaNumeric(peek())) { advance(); }
    std::string text = source.substr(start, current - start);
    //这里写错了
    TokenType type = keywords.contains(text) ? keywords[text] : IDENTIFIER;
    addToken(type);
}
/**
 * @brief 处理 Lox 语言中的数字字面量。
 * 
 * 该函数会持续扫描数字字符，直到遇到非数字字符。
 * 如果遇到小数点且后面跟着数字，则继续扫描小数部分。
 * 最后，提取数字内容并添加到词法单元列表中。
 */
void Scanner::loxnumber() {
    // 持续扫描数字字符
    while (isDigit(peek())) { advance(); }

    // 检查是否有小数部分
    if (peek() == '.' && isDigit(peekNext())) {
        // 消耗小数点
        advance();
        // 持续扫描小数部分的数字字符
        while (isDigit(peek())) { advance(); }
    }
    // 提取数字内容并添加数字词法单元
    addToken(NUMBER, std::string(source.substr(start, current - start)));
}
/**
 * @brief 扫描源代码并生成词法单元（Token）列表。
 * 
 * 该函数会持续扫描源代码，直到到达末尾。在每次循环中，它会将 `start` 指针设置为当前位置，
 * 然后调用 `scanToken()` 函数来处理当前的词法单元。最后，它会添加一个表示文件结束的词法单元，
 * 并返回生成的词法单元列表。
 * 
 * @return 包含所有扫描到的词法单元的 `std::vector<Token>`。
 */
std::vector<Token> Scanner::scanTokens() {
    // 当还未到达源代码末尾时，继续扫描
    while (!isAtEnd()) {
        // 我们总是从下一个字符开始扫描
        start = current;
        // 调用 scanToken 函数处理当前的词法单元
        scanToken();
    }

    // 添加一个表示文件结束的词法单元
    tokens.emplace_back(LoxEOF, "", "", line);
    // 返回扫描到的所有词法单元
    return tokens;
}

/**
 * @brief 扫描单个词法单元并将其添加到词法单元列表中。
 * 
 * 此函数从源代码中读取一个字符，并根据该字符的类型执行相应的操作。
 * 它处理各种符号、关键字、注释、字符串和数字等词法单元。
 * 如果遇到未知字符，将报告错误。
 */
void Scanner::scanToken() {
    // 前进到下一个字符并存储在变量 current_char 中
    char current_char = advance();
    // 根据当前字符进行不同的处理
    switch (current_char) {
        // 处理左括号
        case '(':
            // 添加左括号词法单元
            addToken(LEFT_PAREN);
            break;
        // 处理右括号
        case ')':
            // 添加右括号词法单元
            addToken(RIGHT_PAREN);
            break;
        // 处理左花括号
        case '{':
            // 添加左花括号词法单元
            addToken(LEFT_BRACE);
            break;
        // 处理右花括号
        case '}':
            // 添加右花括号词法单元
            addToken(RIGHT_BRACE);
            break;
        // 处理逗号
        case ',':
            // 添加逗号词法单元
            addToken(COMMA);
            break;
        // 处理点号
        case '.':
            // 添加点号词法单元
            addToken(DOT);
            break;
        // 处理减号
        case '-':
            // 添加减号词法单元
            addToken(MINUS);
            break;
        // 处理加号
        case '+':
            // 添加加号词法单元
            addToken(PLUS);
            break;
        // 处理分号
        case ';':
            // 添加分号词法单元
            addToken(SEMICOLON);
            break;
        // 处理星号
        case '*':
            // 添加星号词法单元
            addToken(STAR);
            break;
        // 处理感叹号
        case '!':
            // 根据后续字符是否为等号，添加不同的词法单元
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
        // 处理等号
        case '=':
            // 根据后续字符是否为等号，添加不同的词法单元
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        // 处理小于号
        case '<':
            // 根据后续字符是否为等号，添加不同的词法单元
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        // 处理大于号
        case '>':
            // 根据后续字符是否为等号，添加不同的词法单元
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;
        // 处理斜杠
        case '/':
            // 如果后续字符也是斜杠，则表示注释
            if (match('/')) {
                // 注释直到行尾
                while (peek() != '\n' && !isAtEnd()) { advance(); }
            } else {
                // 否则添加斜杠词法单元
                addToken(SLASH);
            }
            break;
        // 处理空格、回车和制表符，忽略这些空白字符
        case ' ':
        case '\r':
        case '\t':
            // 忽略空白字符
            break;
        // 处理换行符，行号加 1
        case '\n':
            line++;
            break;
        // 处理双引号，开始处理字符串
        case '"':
            loxstring();
            break;
        // 处理字符 'o'
        case 'o':
            // 如果后续字符是 'r'，则添加 OR 词法单元
            if (match('r')) { addToken(OR); }
            break;
        // 处理其他字符
        default:
            // 如果是数字，处理数字字面量
            if (isDigit(current_char)) {
                loxnumber();
                // 如果是字母，处理标识符
            } else if (isAlpha(current_char)) {
                identifier();
            } else {
                if (isDigit(current_char)) {
                    loxnumber();
                } else {
                    // 遇到未知字符，报告错误
                    error(line, "Unexpected character.");
                }
            }
            break;
    }
}
