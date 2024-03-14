#include "printf.h"

#include <stddef.h>

static size_t fib(size_t n)
{
    if (n < 2)
    {
        return n;
    }
    size_t prev = 0;
    size_t current = 1;
    for (; n > 1; --n)
    {
        size_t next = current + prev;
        prev = current;
        current = next;
    }
    return current;
}

int main(void)
{
    size_t n = 47; // The largest value of n that won't overflow a uint32_t.
    size_t result = fib(n);
    printf("fib(%u) = %u\n", n, result);
    return result;
}
