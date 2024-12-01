#include <stdio.h>
#include <stdint.h>

extern int fib(int n);

int main()
{
    printf("%d\n", fib(45));
    return 0;
}