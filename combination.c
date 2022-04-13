/*
 * Число сочетаний из n по k по простому модулю p
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

/*
 * возвести base в степень power по модулю mod
 */
uint64_t powMod (uint64_t base, uint64_t power, uint64_t mod)
{
    if (power == 0) return 1;
    uint64_t consol = powMod (base, power / 2, mod);
    if (power % 2 == 1) return ((base * consol) % mod) * consol % mod;
    return consol * consol % mod;
}

/*
 * Найти ret: ret * denominator = numerator (mod modulo), modulo - простое
 */
uint64_t divideMod (uint64_t numerator, uint64_t denominator, uint64_t modulo)
{
    return numerator * powMod (denominator, modulo - 2, modulo) % modulo; 
}

int main ()
{
    uint64_t n, k, p;
    scanf ("%llu %llu %llu", &n ,&k, &p);

    if (n == k)
    {
        printf ("1\n");
        return 0;
    }

    if (k > n / 2) k = n - k;
    assert (k <= n - k);
    uint64_t i, k_f = 1;
    for (i = 1; i <= k; i++)
    {
        k_f *= i;
        k_f %= p;
    }
    uint64_t n_m_k_f = k_f;
    for (; i <= n - k; i++)
    {
        n_m_k_f *= i;
        n_m_k_f %= p;
    }
    uint64_t n_f = n_m_k_f;
    for (; i <= n; i++)
    {
        n_f *= i;
        n_f %= p;
    }

    uint64_t denominator = k_f * n_m_k_f % p;
    printf ("%llu\n", divideMod (n_f, denominator, p));

}
