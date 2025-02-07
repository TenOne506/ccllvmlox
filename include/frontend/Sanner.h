// 引入LLVM的SmallVector容器，用于高效存储少量元素
#include <llvm/ADT/SmallVector.h>
// 引入LLVM的StringRef类，用于高效处理字符串引用
#include <llvm/ADT/StringRef.h>

// 引入自定义的Token类头文件
#include "Token.h"
#include <Error/Error.h>
/**
 * @class Scanner
 * @brief 扫描器类，用于将输入的源代码字符串转换为词法单元（Token）序列。
 * 
 * 该类负责对输入的源代码进行逐字符扫描，识别出各种词法单元，如关键字、标识符、常量等，
 * 并将它们存储在一个SmallVector中。扫描过程中会记录当前扫描的位置和行号，以便在出现错误时提供准确的信息。
 */
class Scanner {
private:
    // 存储待扫描的源代码字符串引用
    llvm::StringRef source;
    // 存储扫描得到的词法单元序列
    llvm::SmallVector<Token> tokens;
    // 当前扫描的词法单元的起始位置
    int start = 0;
    // 当前扫描的字符位置
    int current = 0;
    // 当前扫描到的行号
    int line = 1;
    // 标记扫描过程中是否发生错误
    //bool hadError = false;

    //关键字表
    static std::unordered_map<std::string, TokenType> keywords;

    /**
     * @brief 检查扫描是否到达源代码的末尾。
     * 
     * @return 如果到达末尾返回true，否则返回false。
     */
    [[nodiscard]] bool isAtEnd() const;
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, llvm::StringRef literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void loxstring();
    void loxnumber();
    void identifier();


    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);
public:
    /**
     * @brief 扫描源代码并生成词法单元序列。
     * 
     * @return 包含所有扫描到的词法单元的SmallVector。
     */
    llvm::SmallVector<Token> scanTokens();
};