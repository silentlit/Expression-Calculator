//
//  ExpressionParser.cpp
//  Numerical Analysis
//
//  Created by 于京平 on 16/7/22.
//  Copyright © 2016年 于京平. All rights reserved.
//

#include "ExpressionParser.hpp"

ExpressionParser :: ExpressionParser ()
{
    originalExpr = NULL;
    parsedExpr = "";
    p_expr_size = 0;
    var_dic.clear ();
    result = NULL;
}

ExpressionParser :: ExpressionParser (Expression *e)
{
    originalExpr = e;
    parsedExpr = "";
    string temp_e = e->GetExpression ();
    for (string :: iterator iter = temp_e.begin ();iter != temp_e.end ();iter++)
    {
        if (*iter != ' ')
            parsedExpr += *iter;
    }
    p_expr_size = (int)parsedExpr.size ();
    var_dic.clear ();
    result = new CalculationResult ();
}

double ExpressionParser :: GetValue (int l, int r)
{
    double ans = 0;
    pair < int, int > operate = make_pair (INT_MAX, INT_MAX);
    int index = l;
    while (index < r - l + 1)
    {
        index = GetChildExpressionPos (index);
        if (index == p_expr_size - 1)
            break;
        int oprf = OperatorReflection(parsedExpr[++index]);
        if (index >= r - l + 1) break;
        pair < int, int > tempOperate = make_pair (oprf, index);
        operate = min (operate, tempOperate);
        index++;
    }
    int op = operate.first;
    int splitPosition = operate.second;
    if (splitPosition != INT_MAX)
    {
        ans = GetThreeItemOperationValue(op, GetValue (l, splitPosition - 1), GetValue (splitPosition + 1, r));
    }
    else
    {
        if (parsedExpr[l] == '(' && parsedExpr[r] == ')')
        {
            int ll = l + 1, rr = r - 1;
            while (parsedExpr[ll] == '(' && parsedExpr[rr] == ')')
            {
                ll++;
                rr--;
            }
            ans = GetValue (ll, rr);
        }
        else
        {
            bool isFunction = false;
            int functionName_size = 0;
            string temp_e = parsedExpr.substr (l, r - l + 1);
            for (string :: iterator iter = temp_e.begin ();iter != temp_e.end ();iter++)
            {
                functionName_size++;
                if (*iter == '(')
                {
                    isFunction = true;
                    break;
                }
            }
            if (isFunction)
            {
                ans = GetFuncValue(parsedExpr.substr(l, functionName_size - 1), l + functionName_size, r - 1);
            }
            else
            {
                ans = GetSingleValue(l, r);
            }
        }
    }
    return ans;
}

double ExpressionParser :: GetSingleValue (int l, int r)
{
    double ans = 0;
    int index = l;
    double sign = ((parsedExpr[l] == '-') ? -1 : 1);
    if (sign < 0)index++;
    if (isdigit (parsedExpr[index]))
    {
        double integer = 0, decimal = 0;
        int decimalIndex = 0;
        bool isDecimal = false;
        string temp_e = parsedExpr.substr (index, r - index + 1);
        for (string :: iterator iter = temp_e.begin ();iter != temp_e.end ();iter++)
        {
            char c = *iter;
            if (isdigit (c))
            {
                if (!isDecimal)
                {
                    integer = integer * 10 + c - '0';
                }
                else
                {
                    decimal = decimal * 10 + c - '0';
                    decimalIndex++;
                }
            }
            else if (c == '.')
            {
                isDecimal = true;
                continue;
            }
        }
        ans += integer + pow (0.1, decimalIndex) * decimal;
    }
    else
    {
        string varName = parsedExpr.substr (l, r - l + 1);
        if (varName == "pi")
            ans = acos (-1.0);
        else if (varName == "e")
            ans = 2.71828;
    }
    return sign * ans;
}

double ExpressionParser :: GetFuncValue(string funcName, int l, int r)
{
    double ans = 0;
    int index = l;
    double sign = ((parsedExpr[index] == '-') ? -1 : 1);
    if (sign < 0)index++;
    double operatingNum = GetValue (l, r);
    if (funcName == "sin")
    {
        ans = sin (operatingNum);
    }
    else if (funcName == "cos")
    {
        ans = cos (operatingNum);
    }
    else if (funcName == "tan")
    {
        ans = tan (operatingNum);
    }
    else if (funcName == "cot")
    {
        ans = 1.0 / tan (operatingNum);
    }
    else if (funcName == "arcsin")
    {
        ans = asin(operatingNum);
    }
    else if (funcName == "arccos")
    {
        ans = acos(operatingNum);
    }
    else if (funcName == "arctan")
    {
        ans = atan(operatingNum);
    }
    else if (funcName == "arccot")
    {
        ans = atan(1.0 / operatingNum);
    }
    else if (funcName == "abs")
    {
        ans = abs (operatingNum);
    }
    else ans = 0;
    return ans;
}

double ExpressionParser :: GetThreeItemOperationValue(int op, double l, double r)
{
    switch (op)
    {
        case ADD :
            return l + r;
            break;
        case SUB :
            return l - r;
            break;
        case MUL :
            return l * r;
            break;
        case DIV :
            return l / r;
            break;
        case EXP :
            return pow (l, r);
            break;
        default :
            return 0;
    }
}

int ExpressionParser :: GetChildExpressionPos (int start)
{
    int index = start;
    if (parsedExpr[index] == '-')
        index++;
    int frontBracket = 0, backBracket = 0;
    bool isStillInBracket = false;
    for (;index < p_expr_size;index++)
    {
        char c = parsedExpr[index];
        if (isdigit (c) | isalpha (c) || c == '.')
            continue;
        if (c == '(')
        {
            frontBracket++;
            isStillInBracket = true;
        }
        else if (c == ')')
        {
            backBracket++;
            if (frontBracket == backBracket)
                break;
        }
        else if (!isStillInBracket)
        {
            index--;
            break;
        }
    }
    if (index == p_expr_size)
        index--;
    return index;
}

int ExpressionParser :: OperatorReflection (char c)
{
    switch (c)
    {
        case '+' :
            return ADD;
            break;
        case '-' :
            return SUB;
            break;
        case '*' :
            return MUL;
            break;
        case '/' :
            return DIV;
            break;
        case '^' :
            return EXP;
            break;
        default :
            return NONEXIST;
    }
}

void ExpressionParser :: ParseExpression ()
{
    result->SetAllParams(GetValue (0, p_expr_size - 1), true, "HAHA");
}

CalculationResult *ExpressionParser :: GetResult ()
{
    return result;
}