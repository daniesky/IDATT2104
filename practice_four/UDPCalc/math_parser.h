#ifndef MATH_PARSER_H
#define MATH_PARSER_H

#include <stack>
#include <string>

class MathParser {
public:
  static double parse(const std::string &expression) {
    std::stack<double> operands;
    std::stack<char> operators;
    std::string num_str;
    for (char c : expression) {
      if (isdigit(c) || c == '.') {
        num_str += c;
      } else if (c == '+' || c == '-' || c == '*' || c == '/') {
        if (!num_str.empty()) {
          double num = std::stod(num_str);
          operands.push(num);
          num_str.clear();
        }
        while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
          evaluate(operands, operators);
        }
        operators.push(c);
      }
    }
    if (!num_str.empty()) {
      double num = std::stod(num_str);
      operands.push(num);
    }
    while (!operators.empty()) {
      evaluate(operands, operators);
    }
    return operands.top();
  }

private:
  static int precedence(char op) {
    if (op == '+' || op == '-') {
      return 1;
    } else if (op == '*' || op == '/') {
      return 2;
    } else {
      return 0;
    }
  }

  static void evaluate(std::stack<double> &operands, std::stack<char> &operators) {
    char op = operators.top();
    operators.pop();
    double b = operands.top();
    operands.pop();
    double a = operands.top();
    operands.pop();
    double result;
    if (op == '+') {
      result = a + b;
    } else if (op == '-') {
      result = a - b;
    } else if (op == '*') {
      result = a * b;
    } else if (op == '/') {
      result = a / b;
    }
    operands.push(result);
  }
};

#endif // MATH_PARSER_H
