#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cradle.h"

void Term();
int Expression();
void Add();
void Substract();
void Factor();
void Ident();
void Assignment();


void Multiply()
{
    Match('*');
    Factor();
    EmitLn("imull (%esp), %eax");
    /* push of the stack */
    EmitLn("addl $4, %esp");
} 

void Divide()
{
    Match('/');
    Factor();

    /* for a expersion like a/b we have eax=b and %(esp)=a
     * but we need eax=a, and b on the stack 
     */
    EmitLn("movl (%esp), %edx");
    EmitLn("addl $4, %esp");

    EmitLn("pushl %eax");

    EmitLn("movl %edx, %eax");

    /* sign extesnion */
    EmitLn("sarl $31, %edx");
    EmitLn("idivl (%esp)");
    EmitLn("addl $4, %esp");

}

void Ident()
{
    char name = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        sprintf(tmp, "call %c", name);
        EmitLn(tmp);
    } else {
        sprintf(tmp, "movl %c, %%eax", name);
        EmitLn(tmp);
    }
}

void Factor()
{
    if(Look == '(') {
        Match('(');
        Expression();
        Match(')');
     } else if(IsAddop(Look)) {
        Match('-');
        sprintf(tmp,"movl $%c, %%eax", GetNum());
        EmitLn(tmp);
        EmitLn("negl %eax");
    } else if (IsAlpha(Look)) {
        Ident();
    } else {
        sprintf(tmp,"movl $%c, %%eax", GetNum());
        EmitLn(tmp);
    }
}

void Term()
{
    Factor();
    while (strchr("*/", Look)) {

        EmitLn("pushl %eax");

        switch(Look)
        {
            case '*':
                Multiply();
                break;
            case '/':
                Divide();
                break;
            default:
                Expected("Mulop");
        }
    }
}

int Expression()
{
    return GetNum();
    /* if(IsAddop(Look)) */
    /*     EmitLn("xor %eax, %eax"); */
    /* else */
    /*     Term(); */

    /* while (strchr("+-", Look)) { */

    /*     EmitLn("pushl %eax"); */

    /*     switch(Look) */
    /*     { */
    /*         case '+': */
    /*             Add(); */
    /*             break; */
    /*         case '-': */
    /*             Substract(); */
    /*             break; */
    /*         default: */
    /*             Expected("Addop"); */
    /*     } */
    /* } */
}


void Add()
{
    Match('+');
    Term();
    EmitLn("addl (%esp), %eax");
    EmitLn("addl $4, %esp");
    
}


void Substract()
{
    Match('-');
    Term();
    EmitLn("subl (%esp), %eax");
    EmitLn("negl %eax");
    EmitLn("addl $4, %esp");
}

void Assignment()
{
    char name = GetName();
    Match('=');
    Expression();
    sprintf(tmp, "lea %c, %%ebx", name);
    EmitLn(tmp);
    EmitLn("movl %eax, (%ebx)");
}

int main()
{

    Init();
    /* Expression(); */
    /* Assignment(); */
    
    printf("%d\n", Expression());
    if (Look != '\n') {
        Expected("NewLine");
    }

    return 0;
}
