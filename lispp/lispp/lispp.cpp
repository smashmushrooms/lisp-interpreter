#include "lispp.h"
#include <exception>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>

Tokenizer::Tokenizer(std::istream* input_stream)
        : input_stream_(input_stream) {}

void Tokenizer::ReadNext() {
    if (input_stream_->peek() == EOF) {
        type_ = TokenType::END_OF_FILE;
        return;
    }

    *input_stream_ >> std::ws;

    char symb;
    *input_stream_ >> symb;
    std::string token = std::string();
    token.push_back(symb);

    if (!(symb == '(' || symb == ')')) {
        char next = input_stream_->peek();
        while (next && next != ' ' && next != '(' && next != ')' && next != EOF) {
            token.push_back(input_stream_->get());
            next = input_stream_->peek();
        }
    } else {
        type_ = ((symb == '(') ? TokenType::OPEN_PARENT : TokenType::CLOSE_PARENT);
        return;
    }

    if (SetNumber(token)) {
        size_t *processed = nullptr;
        number_ = std::stoll(token, processed, 10);
        type_ = TokenType::NUMBER;
    } else if (IsName(token)) {
        name_ = token;
        type_ = TokenType::NAME;
    } else {
        switch (symb) {
            case '(':
                type_ = TokenType::OPEN_PARENT;
                break;
            case ')':
                type_ = TokenType::CLOSE_PARENT;
                break;
            case '#':
                type_ = TokenType::SHARP;
                break;
            case '.':
                type_ = TokenType::PAIR;
                break;
            case '\'':
                type_ = TokenType::APOSTROPH;
                break;
            default:
                type_ = TokenType::UNKNOWN;
                break;
        }
    }
}

Tokenizer::TokenType Tokenizer::ShowTokenType() {
    return type_;
}

const std::string& Tokenizer::GetTokenName() {
    return name_;
}

int64_t Tokenizer::GetTokenNumber() {
    return number_;
}

bool Tokenizer::SetNumber(const std::string &token) {
    try {
        size_t processed = 0;
        std::stoll(token, &processed, 10);
        if (processed != token.size()) {
            return false;
        }
    } catch (const std::invalid_argument& exc) {
        return false;
    } catch (const std::out_of_range& exc) {
        return false;
    }

    return true;
}

bool Tokenizer::IsName(const std::string& token) {
    bool return_value = false;
    if (builtins_.find(token) != builtins_.end()) {
        return_value = true;
    }

    if (!token.empty() && std::isalpha(token[0])) {
        auto iter = token.begin();
        while (iter < token.end()) {
            if (!std::isalnum(*iter)) {
                break;
            }
            ++iter;
        }
        if (iter == token.end()) {
            return_value = true;
        }
    }

    return return_value;
}
