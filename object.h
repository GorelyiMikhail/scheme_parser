#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "error.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Eval() {
        throw RuntimeError();
    }

    virtual std::shared_ptr<Object> Apply(std::shared_ptr<Object>) {
        throw RuntimeError();
    }

    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value) : value_(value) {
    }

    std::shared_ptr<Object> Eval() override {
        return std::make_shared<Number>(value_);
    }

    int GetValue() const {
        return value_;
    }

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& value) : value_(value) {
    }

    std::shared_ptr<Object> Eval() override;

    const std::string& GetName() const {
        return value_;
    }

private:
    const std::string value_;
};

class Boolean : public Object {
public:
    Boolean(bool value) : value_(value) {
    }

    std::shared_ptr<Object> Eval() override;

    bool GetValue() const {
        return value_;
    }

private:
    bool value_;
};

class Function : public Object {};

class BooleanPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class NumberPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Pair : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Null : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class ListPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Cons : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Car : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class List : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Ref : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Tail : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Sum : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Sub : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Mul : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Div : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Equ : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Les : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Gre : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Loe : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Goe : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Min : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Max : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Abs : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Not : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class And : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Or : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Quote : public Function {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> head) override;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }

    std::shared_ptr<Object> Eval() override;

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

private:
    std::shared_ptr<Object> first_, second_;
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (!std::dynamic_pointer_cast<T>(obj)) {
        return false;
    } else {
        return true;
    }
}

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    if (Is<T>(obj)) {
        return std::static_pointer_cast<T>(obj);
    } else {
        return nullptr;
    }
}

std::vector<std::shared_ptr<Object>> ToVector(const std::shared_ptr<Object>&);

template <class T>
void ValidateArgsType(std::vector<std::shared_ptr<Object>>);

template <class Result, class T, class F, class It>
std::shared_ptr<Result> Fold(It, It, T, F);
