#include <iostream>
#include <sstream>
#include <stack>
#include <cmath>
#include <map>
#include <cctype>
#include <stdexcept>
#include <vector>
#include "bigint.h"
#include "bigint_extra.h"
#include "timing.hpp"


namespace evl{
enum EvalVersion {BIGINT,DOUBLE,LOGIC,BOOLEAN};
enum TokenType {NUMBER, OPERATOR, FUNCTION, PARENTHESIS};
// Можно добавить константы
std::map<std::string,std::string> constants_math={{"pi","3.141592653589793"},{"e","2.718281828459045"},{"phi","1.618033988749895"}};


static bool TIME_TESTING=false;
static bool TRIG_MODE_DEG=false;
const double DEG_TO_RAD = 0.01745329251994329576923690768488612713412398;

struct Token {
    TokenType type;
    std::string value;
};

//Токенайзер
std::vector<Token> tokenize(const std::string& expr){
    if(TIME_TESTING){time();}
    std::vector<Token> tokens;
    std::string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char ch = expr[i];

        if (isspace(ch)) continue;

        if (isdigit(ch) || ch == '.') {
            bool has_dot=(ch=='.');
            num += ch;
            while (i + 1 < expr.size() && (isdigit(expr[i + 1]) || expr[i + 1] == '.')){
                if(expr[i+1]=='.'){
                 if(has_dot){throw std::runtime_error("Double .");}else{has_dot=true;}   
                }
                num += expr[++i];
            }
            tokens.push_back({NUMBER, num});
            num.clear();
        }
        else if (isalpha(ch)) {
            std::string func;
            func += ch;
            while (i + 1 < expr.size() && isalpha(expr[i + 1]))
                func += expr[++i];
            if(constants_math.find(func)!=constants_math.end()){
                tokens.push_back({NUMBER, constants_math[func]});
            } else {
            tokens.push_back({FUNCTION, func});
            }
        }
        //Отдельна обработка минуса для определения унарного
        else if (ch == '-') {
            if (tokens.empty() || 
                tokens.back().type == OPERATOR || 
                (tokens.back().type == PARENTHESIS && tokens.back().value == "(")) {
                tokens.push_back({OPERATOR, "~"}); // унарный минус
            } else {
                tokens.push_back({OPERATOR, "-"});
            }
        }
        else if (ch == '+' || ch == '*' || ch == '/' || ch == '%' || ch == '^' || ch == '!') {
            tokens.push_back({OPERATOR, std::string(1, ch)});
        }
        //Скобки
        else if (ch == '(' || ch == ')') {
            tokens.push_back({PARENTHESIS, std::string(1, ch)});
        }
        else {
            throw std::runtime_error("Unknown character in expression");
        }
    }

    return tokens;
}

int precedence(const std::string& op) {
    if (op == "!" || op == "~") return 5;
    if (op == "^") return 4;
    if (op == "*" || op == "/" || op == "%") return 3;
    if (op == "+" || op == "-") return 2;
    return 0;
}

bool isRightAssociative(const std::string& op) {
    return op == "^" || op == "!" || op == "~";
}

std::vector<Token> toPostfix(const std::vector<Token>& tokens){
    std::vector<Token> output;
    std::stack<Token> ops;
    
    for (const auto& token : tokens) {
        if (token.type == NUMBER) {
            output.push_back(token);
        }
        else if (token.type == FUNCTION) {
            ops.push(token);
        }
        else if (token.type == OPERATOR) {
            while (!ops.empty() && (
                (ops.top().type == FUNCTION) ||
                (ops.top().type == OPERATOR &&
                 (precedence(ops.top().value) > precedence(token.value) ||
                  (precedence(ops.top().value) == precedence(token.value) &&
                   !isRightAssociative(token.value)))
                )
            )) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        }
        else if (token.value == "(") {
            ops.push(token);
        }
        else if (token.value == ")") {
            while (!ops.empty() && ops.top().value != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty()) ops.pop(); // remove '('
            if (!ops.empty() && ops.top().type == FUNCTION) {
                output.push_back(ops.top());
                ops.pop();
            }
        }
    }

    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

double evalPostfix_double(const std::vector<Token>& postfix) {
    std::stack<double> stk;

    for (const auto& token : postfix) {
        if (token.type == NUMBER) {
            stk.push(stod(token.value));
        }
        else if (token.type == OPERATOR || token.type == FUNCTION) {
            if (token.value == "+") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                stk.push(a + b);
            } else if (token.value == "-") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                stk.push(a - b);
            } else if (token.value == "*") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                stk.push(a * b);
            } else if (token.value == "/") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                if (b == 0) throw std::runtime_error("Деление на ноль");
                stk.push(a / b);
            } else if (token.value == "%") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                stk.push(fmod(a, b));
            } else if (token.value == "^") {
                double b = stk.top(); stk.pop();
                double a = stk.top(); stk.pop();
                stk.push(pow(a, b));
            } else if (token.value == "~") {
                double a = stk.top(); stk.pop();
                stk.push(-a);
            } else {
                    //бинарные функции
                if (token.value == "log") {
                    double arg = stk.top(); stk.pop();
                    double base = stk.top(); stk.pop();
                    if (arg <= 0 || base <= 0 || base == 1)
                        throw std::runtime_error("Недопустимые значения для log(base, arg)");
                    stk.push(log(arg) / log(base));
                } else {
                    //унарные функции
                    double a = stk.top(); stk.pop();

                    if (token.value == "sin") {
                        if(TRIG_MODE_DEG){
                        stk.push(sin(a*DEG_TO_RAD));    
                        } else {
                        stk.push(sin(a));
                    }
                    } else if (token.value == "cos") {
                        if(TRIG_MODE_DEG){
                        stk.push(cos(a*DEG_TO_RAD));    
                        } else {
                        stk.push(cos(a));
                    }
                    }
                    else if (token.value == "tan" || token.value == "tg") {stk.push(tan(a));}
                    else if (token.value == "ctg") {stk.push(1.0 / tan(a));}
                    else if (token.value == "arcsin") {
                        if (a < -1 || a > 1) throw std::runtime_error("arcsin out of domain");
                        stk.push(asin(a));
                    }
                    else if (token.value == "arccos") {
                        if (a < -1 || a > 1) throw std::runtime_error("arccos out of domain");
                        stk.push(acos(a));
                    }
                    else if (token.value == "arctan" || token.value == "arctg") {stk.push(atan(a));}
                    else if (token.value == "arcctg" || token.value == "arccot") {
                        if (a == 0) throw std::runtime_error("arcctg undefined for zero");
                        stk.push(atan(1.0 / a));
                    }
                    else if (token.value == "ln") stk.push(log(a));
                    else if (token.value == "lg") stk.push(log10(a));
                    else if (token.value == "sqrt") stk.push(sqrt(a));
                    else if (token.value == "abs") stk.push(abs(a));
                    else throw std::runtime_error("Неизвестная функция: " + token.value);
                }
            }
        }
    }

    if (stk.size() != 1) throw std::runtime_error("Некорректное выражение");
    if(TIME_TESTING){time();}
    return stk.top();
}


bigint evalPostfix_bigint(const std::vector<Token>& postfix) {
    std::stack<bigint> stk;

    for (const auto& token : postfix) {
        if (token.type == NUMBER) {
            stk.push(bigint(token.value));
        }
        else if (token.type == OPERATOR || token.type == FUNCTION) {
            if (token.value == "+") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                stk.push(a + b);
            } else if (token.value == "-") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                stk.push(a - b);
            } else if (token.value == "*") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                stk.push(a * b);
            } else if (token.value == "/") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                if (b.isZero()) throw std::runtime_error("Деление на ноль");
                stk.push(a / b);
            } else if (token.value == "%") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                stk.push(a%b);
            } else if (token.value == "^") {
                bigint b = stk.top(); stk.pop();
                bigint a = stk.top(); stk.pop();
                stk.push(a.to_pow(b));
            } else if (token.value == "!") {
                bigint a = stk.top(); stk.pop();
                stk.push(factorial(a));
            } else if (token.value == "~") {
                bigint a = stk.top(); stk.pop();
                stk.push(-a);
            } else {
                    //бинарные функции
                if (token.value == "gcd") {
                    bigint a = stk.top(); stk.pop();
                    bigint b = stk.top(); stk.pop();
                    stk.push(gcd(a,b));
                } else {
                    //унарные функции
                    bigint a = stk.top(); stk.pop();

                    if (token.value == "sqrt") stk.push(sqrt(a));
                    else if (token.value == "abs") stk.push(abs(a));
                    else throw std::runtime_error("Неизвестная функция: " + token.value);
                }
            }
        }
    }

    if (stk.size() != 1) throw std::runtime_error("Некорректное выражение");
    if(TIME_TESTING){time();}
    return stk.top();
}

double evaluate_double(const std::string& expression) {

    auto tokens = tokenize(expression);
    auto postfix = toPostfix(tokens);
    return evalPostfix_double(postfix);
}

bigint evaluate_bigint(const std::string& expression) {

    auto tokens = tokenize(expression);
    auto postfix = toPostfix(tokens);
    return evalPostfix_bigint(postfix);
}

/// enters calculator mode. Simple API for external use
void calculator_mode(){
    std::cout<<"Evaluation mode. By default evaluates integer numbers. To evaluate doubles enter [double] or [d]. To change trigonometric functions mode use [rad] and [deg]\n\n";
        bool eval_state=true;
        bool double_eval=false;
        std::string text_input;
        while(eval_state){
            std::getline(std::cin,text_input);
            if(text_input.size()==0 || text_input==" "){continue;};
            if(text_input=="exit" || text_input=="q" || text_input=="quit"){
                eval_state=false;
            }
            if(text_input=="double" || text_input=="d"){double_eval=true;continue;}
            if(text_input=="int"||text_input=="bigint"){double_eval=false;continue;}
            if(text_input=="deg"){evl::TRIG_MODE_DEG=true;continue;}
            if(text_input=="rad"){evl::TRIG_MODE_DEG=false;continue;}
            if(text_input=="clear"){system("cls");continue;}
            try{
            if(double_eval){std::cout<<evl::evaluate_double(text_input)<<"\n";}else{std::cout<<evl::evaluate_bigint(text_input)<<"\n";}
            } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            }
        }
        return;
}



}