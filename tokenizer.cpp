#include "tokenizer.h"
#include "error.h"

std::unordered_set<char> special = {'<', '=', '>', '*', '/', '#'};
std::unordered_set<char> enlarged = {'?', '!', '-'};

Tokenizer::Tokenizer(std::istream *in) : in_(in) {
    Next();
}

void Tokenizer::Next() {
    while (std::isspace(in_->peek())) {
        in_->get();
    }
    if (in_->eof()) {
        end_ = true;
        return;
    }
    char cur_char = in_->get();
    if (cur_char == '(') {
        cur_token_ = BracketToken::OPEN;
    } else if (cur_char == ')') {
        cur_token_ = BracketToken::CLOSE;
    } else if (cur_char == '.') {
        cur_token_ = DotToken();
    } else if (cur_char == '\'') {
        cur_token_ = QuoteToken();
    } else if (std::isalpha(cur_char) || special.contains(cur_char) ||
               ((cur_char == '-' || cur_char == '+') && !std::isdigit(in_->peek()))) {
        std::string new_token;
        new_token.push_back(cur_char);
        while (std::isalpha(in_->peek()) || std::isdigit(in_->peek()) ||
               special.contains(in_->peek()) || enlarged.contains(in_->peek())) {
            new_token.push_back(in_->get());
        }
        cur_token_ = SymbolToken{new_token};
    } else if (((cur_char == '-' || cur_char == '+') && std::isdigit(in_->peek())) ||
               (std::isdigit(cur_char))) {
        std::string new_token;
        new_token.push_back(cur_char);
        while (std::isdigit(in_->peek())) {
            new_token.push_back(in_->get());
        }
        cur_token_ = ConstantToken{std::stoi(new_token)};
    } else {
        throw SyntaxError();
    }
}

Token Tokenizer::GetToken() {
    return cur_token_;
}

bool Tokenizer::IsEnd() {
    return end_;
}

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name_ == other.name_;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value_ == other.value_;
}