#include "object.h"

std::unordered_map<std::string, std::shared_ptr<Function>> functions = {
    {"boolean?", std::make_shared<BooleanPredicate>()},
    {"number?", std::make_shared<NumberPredicate>()},
    {"pair?", std::make_shared<Pair>()},
    {"null?", std::make_shared<Null>()},
    {"list?", std::make_shared<ListPredicate>()},
    {"cons", std::make_shared<Cons>()},
    {"car", std::make_shared<Car>()},
    {"cdr", std::make_shared<Cdr>()},
    {"list", std::make_shared<List>()},
    {"list-ref", std::make_shared<Ref>()},
    {"list-tail", std::make_shared<Tail>()},
    {"+", std::make_shared<Sum>()},
    {"-", std::make_shared<Sub>()},
    {"*", std::make_shared<Mul>()},
    {"/", std::make_shared<Div>()},
    {"=", std::make_shared<Equ>()},
    {"<", std::make_shared<Les>()},
    {">", std::make_shared<Gre>()},
    {"<=", std::make_shared<Loe>()},
    {">=", std::make_shared<Goe>()},
    {"min", std::make_shared<Min>()},
    {"max", std::make_shared<Max>()},
    {"abs", std::make_shared<Abs>()},
    {"not", std::make_shared<Not>()},
    {"and", std::make_shared<And>()},
    {"or", std::make_shared<Or>()},
    {"quote", std::make_shared<Quote>()},
};

std::vector<std::shared_ptr<Object>> ToVector(const std::shared_ptr<Object>& head) {
    std::vector<std::shared_ptr<Object>> result;
    if (!head) {
        return result;
    } else if (Is<Cell>(head)) {
        std::vector<std::shared_ptr<Object>> v = ToVector(As<Cell>(head)->GetFirst());
        std::copy(v.begin(), v.end(), std::back_inserter(result));
        v = ToVector(As<Cell>(head)->GetSecond());
        std::copy(v.begin(), v.end(), std::back_inserter(result));
    } else if (Is<Number>(head)) {
        result.push_back(As<Number>(head));
    } else if (Is<Symbol>(head)) {
        result.push_back(As<Symbol>(head));
    } else if (Is<Boolean>(head)) {
        result.push_back(As<Boolean>(head));
    }
    return result;
}

template <class T>
void ValidateArgsType(std::vector<std::shared_ptr<Object>> data) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (!Is<T>(data[i])) {
            throw RuntimeError();
        }
    }
}

template <class Result, class T, class F, class It>
std::shared_ptr<Result> Fold(It begin, It end, T init, F f) {
    T result = init;
    while (begin != end) {
        result = f(result, As<Result>(*begin)->GetValue());
        ++begin;
    }
    return std::make_shared<Result>(result);
}

std::shared_ptr<Object> Symbol::Eval() {
    if (functions.contains(GetName())) {
        return functions.at(GetName());
    } else if (GetName() == "#t") {
        return std::make_shared<Boolean>(true);
    } else if (GetName() == "#f") {
        return std::make_shared<Boolean>(false);
    } else {
        return std::make_shared<Symbol>(value_);
    }
}

std::shared_ptr<Object> Boolean::Eval() {
    return std::make_shared<Boolean>(value_);
}

std::shared_ptr<Object> Cell::Eval() {
    if (!first_) {
        throw RuntimeError();
    }
    auto temp1 = first_->Eval();
    if (Is<Function>(temp1)) {
        return temp1->Apply(second_);
    } else {
        auto temp2 = second_;
        if (second_) {
            temp2 = second_->Eval();
        }
        return std::make_shared<Cell>(temp1, temp2);
    }
}

std::shared_ptr<Object> BooleanPredicate::Apply(std::shared_ptr<Object> head) {
    return std::make_shared<Boolean>(Is<Boolean>(As<Cell>(head)->GetFirst()->Eval()));
}

std::shared_ptr<Object> NumberPredicate::Apply(std::shared_ptr<Object> head) {
    return std::make_shared<Boolean>(Is<Number>(As<Cell>(head)->GetFirst()->Eval()));
}

std::shared_ptr<Object> Sum::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Number>(0);
    } else if (vec.size() == 1) {
        return vec[0];
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return x + y; });
    }
}

std::shared_ptr<Object> Sub::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        throw RuntimeError();
    } else if (vec.size() == 1) {
        return std::make_shared<Number>(-1 * As<Number>(vec[0])->GetValue());
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return x - y; });
    }
}
std::shared_ptr<Object> Mul::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Number>(1);
    } else if (vec.size() == 1) {
        return vec[0];
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return x * y; });
    }
}
std::shared_ptr<Object> Div::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        throw RuntimeError();
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return x / y; });
    }
}

std::shared_ptr<Object> Equ::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        for (size_t i = 1; i < vec.size(); ++i) {
            if (As<Number>(vec[0])->GetValue() != As<Number>(vec[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> Les::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        for (size_t i = 1; i < vec.size(); ++i) {
            if (As<Number>(vec[0])->GetValue() >= As<Number>(vec[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> Gre::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        for (size_t i = 1; i < vec.size(); ++i) {
            if (As<Number>(vec[0])->GetValue() <= As<Number>(vec[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> Loe::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        for (size_t i = 1; i < vec.size(); ++i) {
            if (As<Number>(vec[0])->GetValue() > As<Number>(vec[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> Goe::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else if (vec.size() == 1) {
        throw RuntimeError();
    } else {
        for (size_t i = 1; i < vec.size(); ++i) {
            if (As<Number>(vec[0])->GetValue() < As<Number>(vec[i])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        }
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> Min::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        throw RuntimeError();
    } else if (vec.size() == 1) {
        return vec[0];
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return std::min(x, y); });
    }
}

std::shared_ptr<Object> Max::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        throw RuntimeError();
    } else if (vec.size() == 1) {
        return vec[0];
    } else {
        return Fold<Number>(vec.begin() + 1, vec.end(), As<Number>(vec[0])->GetValue(),
                            [](int64_t x, int64_t y) { return std::max(x, y); });
    }
}

std::shared_ptr<Object> Abs::Apply(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vec;
    if (head) {
        vec = ToVector(head->Eval());
    }
    ValidateArgsType<Number>(vec);
    if (vec.size() == 0) {
        throw RuntimeError();
    } else if (vec.size() == 1) {
        return std::make_shared<Number>(std::abs(As<Number>(vec[0])->GetValue()));
    } else {
        throw RuntimeError();
    }
}

std::shared_ptr<Object> Not::Apply(std::shared_ptr<Object> head) {
    if (!head) {
        throw RuntimeError();
    }
    if (As<Cell>(head->Eval())->GetSecond()) {
        throw RuntimeError();
    }
    if (Is<Boolean>(As<Cell>(head)->GetFirst()->Eval())) {
        return std::make_shared<Boolean>(
            !As<Boolean>(As<Cell>(head)->GetFirst()->Eval())->GetValue());
    } else {
        return std::make_shared<Boolean>(false);
    }
}

std::shared_ptr<Object> And::Apply(std::shared_ptr<Object> head) {
    if (!head) {
        return std::make_shared<Boolean>(true);
    } else if (Is<Cell>(head)) {
        if (!Is<Boolean>(Apply(As<Cell>(head)->GetFirst()->Eval())) ||
            As<Boolean>(Apply(As<Cell>(head)->GetFirst()->Eval()))->GetValue()) {
            if (As<Cell>(head)->GetSecond()) {
                return Apply(As<Cell>(head)->GetSecond());
            } else {
                return As<Cell>(head)->GetFirst()->Eval();
            }
        } else {
            return std::make_shared<Boolean>(false);
        }
    } else {
        return head;
    }
}

std::shared_ptr<Object> Or::Apply(std::shared_ptr<Object> head) {
    if (!head) {
        return std::make_shared<Boolean>(false);
    } else if (Is<Cell>(head)) {
        if (!Is<Boolean>(Apply(As<Cell>(head)->GetFirst()->Eval())) ||
            !As<Boolean>(Apply(As<Cell>(head)->GetFirst()->Eval()))->GetValue()) {
            if (As<Cell>(head)->GetSecond()) {
                return Apply(As<Cell>(head)->GetSecond());
            } else {
                return As<Cell>(head)->GetFirst()->Eval();
            }
        } else {
            return std::make_shared<Boolean>(true);
        }
    } else {
        return head->Eval();
    }
}

std::shared_ptr<Object> Quote::Apply(std::shared_ptr<Object> head) {
    return head;
}

std::shared_ptr<Object> Pair::Apply(std::shared_ptr<Object> head) {
    auto vec = ToVector(As<Cell>(head)->GetFirst()->Eval());
    if (vec.size() == 2) {
        return std::make_shared<Boolean>(true);
    } else {
        return std::make_shared<Boolean>(false);
    }
}

std::shared_ptr<Object> Null::Apply(std::shared_ptr<Object> head) {
    auto vec = ToVector(As<Cell>(head)->GetFirst()->Eval());
    if (vec.size() == 0) {
        return std::make_shared<Boolean>(true);
    } else {
        return std::make_shared<Boolean>(false);
    }
}

std::shared_ptr<Object> ListPredicate::Apply(std::shared_ptr<Object> head) {
    auto cur = As<Cell>(head)->GetFirst()->Eval();
    if (!cur) {
        return std::make_shared<Boolean>(true);
    }
    while (As<Cell>(cur)->GetSecond() && Is<Cell>(As<Cell>(cur)->GetSecond())) {
        cur = As<Cell>(cur)->GetSecond();
    }
    if (!As<Cell>(cur)->GetSecond()) {
        return std::make_shared<Boolean>(true);
    } else {
        return std::make_shared<Boolean>(false);
    }
}

std::shared_ptr<Object> Cons::Apply(std::shared_ptr<Object> head) {
    return std::make_shared<Cell>(As<Cell>(head->Eval())->GetFirst(),
                                  As<Cell>(As<Cell>(head->Eval())->GetSecond())->GetFirst());
}
std::shared_ptr<Object> Car::Apply(std::shared_ptr<Object> head) {
    auto vec = ToVector(head->Eval());
    if (vec.size() == 0) {
        throw RuntimeError();
    }
    return As<Number>(vec[0]);
}
std::shared_ptr<Object> Cdr::Apply(std::shared_ptr<Object> head) {
    auto temp = As<Cell>(head->Eval())->GetFirst();
    if (!temp) {
        throw RuntimeError();
    }
    return As<Cell>(temp)->GetSecond();
}

std::shared_ptr<Object> List::Apply(std::shared_ptr<Object> head) {
    if (!head) {
        return nullptr;
    }
    auto vec = ToVector(head->Eval());
    auto ans = std::make_shared<Cell>(As<Number>(vec.back()), nullptr);
    vec.pop_back();
    while (vec.size() > 0) {
        ans = std::make_shared<Cell>(As<Number>(vec.back()), ans);
        vec.pop_back();
    }
    return ans;
}

std::shared_ptr<Object> Ref::Apply(std::shared_ptr<Object> head) {
    auto vec = ToVector(As<Cell>(head->Eval())->GetFirst());
    auto temp = As<Cell>(head->Eval())->GetSecond();
    if (vec.size() <= As<Number>(As<Cell>(temp)->GetFirst())->GetValue()) {
        throw RuntimeError();
    }
    return As<Number>(vec[As<Number>(As<Cell>(temp)->GetFirst())->GetValue()]);
}

std::shared_ptr<Object> Tail::Apply(std::shared_ptr<Object> head) {
    auto ans = As<Cell>(head->Eval())->GetFirst();
    for (int i = 0;
         i < As<Number>(As<Cell>(As<Cell>(head->Eval())->GetSecond())->GetFirst())->GetValue() - 1;
         ++i) {
        if (!As<Cell>(ans)->GetSecond()) {
            throw RuntimeError();
        }
        ans = As<Cell>(ans)->GetSecond();
    }
    return As<Cell>(ans)->GetSecond();
}
