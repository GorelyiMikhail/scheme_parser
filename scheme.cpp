#include "scheme.h"

std::string PrintCell(std::shared_ptr<Object> obj) {
    std::string result;
    if (!obj) {
        return result;
    }
    if (!As<Cell>(obj)->GetFirst()) {
        result += "()";
    } else if (Is<Number>(As<Cell>(obj)->GetFirst())) {
        result += std::to_string(As<Number>(As<Cell>(obj)->GetFirst())->GetValue());
    } else if (Is<Symbol>(As<Cell>(obj)->GetFirst())) {
        result += As<Symbol>(As<Cell>(obj)->GetFirst())->GetName();
    } else if (Is<Boolean>(As<Cell>(obj)->GetFirst())) {
        result += As<Boolean>(As<Cell>(obj)->GetFirst())->GetValue() ? "#t" : "#f";
    } else if (Is<Cell>(As<Cell>(obj)->GetFirst())) {
        result += PrintCell(As<Cell>(obj)->GetFirst());
    }
    if (!As<Cell>(obj)->GetSecond()) {
        return result;
    } else if (Is<Number>(As<Cell>(obj)->GetSecond())) {
        result += (As<Cell>(obj)->GetFirst() ? " . " : "") +
                  std::to_string(As<Number>(As<Cell>(obj)->GetSecond())->GetValue());
    } else if (Is<Symbol>(As<Cell>(obj)->GetSecond())) {
        result += (As<Cell>(obj)->GetFirst() ? " . " : "") +
                  As<Symbol>(As<Cell>(obj)->GetSecond())->GetName();
    } else if (Is<Boolean>(As<Cell>(obj)->GetSecond())) {
        result += (As<Cell>(obj)->GetFirst() ? " . " : "");
        result += (As<Boolean>(As<Cell>(obj)->GetSecond())->GetValue() ? "#t" : "#f");
    } else if (Is<Cell>(As<Cell>(obj)->GetSecond())) {
        result += (As<Cell>(obj)->GetFirst() ? " " : "") + PrintCell(As<Cell>(obj)->GetSecond());
    }
    return result;
}

std::string Interpreter::Run(const std::string str) {
    std::stringstream ss{str};
    Tokenizer tokenizer{&ss};
    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError();
    }
    if (!obj) {
        throw RuntimeError();
    }
    auto result = obj->Eval();
    if (!result) {
        return "()";
    } else if (Is<Number>(result)) {
        return std::to_string(As<Number>(result)->GetValue());
    } else if (Is<Boolean>(result)) {
        return As<Boolean>(result)->GetValue() ? "#t" : "#f";
    } else if (Is<Cell>(result) && As<Cell>(obj)->GetFirst() &&
               Is<Function>(As<Cell>(obj)->GetFirst()->Eval())) {
        return "(" + PrintCell(result) + ")";
    } else {
        throw RuntimeError();
    }
}
