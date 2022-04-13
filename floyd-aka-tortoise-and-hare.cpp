/*
 * Использование алгоритма Флойда для перевода обыкновенной дроби в десятичное периодическое представление
 */

#include <iostream>
#include <string>

struct NextDigit
{
    int operator () (int x, int m)
    {
        return 10 * x % m;
    }
};

template <typename Param>
struct FuncTraits
{
    Param period, predperiod;
};

template <typename Param, typename Func, typename... Args>
FuncTraits<Param> floyd (Param x0, Args&... args)
{
    FuncTraits<Param> ret { };

    Param tortoise = x0;
    Param hare = x0;

    do 
    {
        tortoise = Func() (tortoise, args...);
        hare = Func()(Func() (hare, args...), args...);
    } while (tortoise != hare);

    hare = x0;
    
    while (tortoise != hare)
    {
        hare = Func() (hare, args...);
        tortoise = Func() (tortoise, args...);
        ret.predperiod++;
    }

    do
    {
        hare = Func() (hare, args...);
        ret.period++;
    } while (tortoise != hare);
    
    return ret;

}

int main ()
{
    int numerator,denominator;
    scanf ("%d %d", &numerator, &denominator);

    printf ("%d,", numerator / denominator);
    
    numerator = numerator % denominator;

    auto [period, predperiod] = floyd <int, NextDigit, int> (numerator, denominator);

    for (int i = 0; i < predperiod; i++)
    {
        printf ("%d", 10 * numerator / denominator);
        numerator = NextDigit() (numerator, denominator);
    }

    if (numerator == 0 && period == 1) 
    {
        printf ("\n");
        return 0;
    }
    printf ("(");
    for (int i = 0; i < period; i++)
    {
        printf ("%d", 10 * numerator / denominator);
        numerator = NextDigit() (numerator, denominator);
    }
    printf (")\n");
}
