#include "parser.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    Token cur_token = tokenizer->GetToken();
    if (tokenizer->IsEnd()) {
        throw SyntaxError();
    } else if (cur_token.index() == 0) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(cur_token).value_);
    } else if (cur_token == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        return ReadList(tokenizer);
    } else if (cur_token.index() == 2) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(cur_token).name_);
    } else if (cur_token.index() == 3) {
        tokenizer->Next();
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), Read(tokenizer));
    } else {
        throw SyntaxError();
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    }
    auto first = Read(tokenizer);
    if (tokenizer->IsEnd()) {
        throw SyntaxError();
    }
    if (tokenizer->GetToken().index() == 0) {
        return std::make_shared<Cell>(first, ReadList(tokenizer));
    } else if (tokenizer->GetToken().index() == 1) {
        return std::make_shared<Cell>(first, ReadList(tokenizer));
    } else if (tokenizer->GetToken().index() == 2) {
        return std::make_shared<Cell>(first, ReadList(tokenizer));
    } else if (tokenizer->GetToken().index() == 3) {
        return std::make_shared<Cell>(first, ReadList(tokenizer));
    } else if (tokenizer->GetToken().index() == 4) {
        tokenizer->Next();
        auto second = Read(tokenizer);
        if (tokenizer->GetToken() != Token{BracketToken::CLOSE} || tokenizer->IsEnd()) {
            throw SyntaxError();
        }
        tokenizer->Next();
        return std::make_shared<Cell>(first, second);
    } else {
        throw SyntaxError();
    }
}
