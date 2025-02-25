#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int pwr(int a, int b)
{
    int tem = 1;
    while (b > 0)
    {
        if (b & 1)
            tem = tem * a;
        a = a * a;
        b >>= 1;
    }
    return tem;
}

void mprn(int *MEM, int i)
{
    printf("+++ Mem[%d] set to %d\n", i, MEM[i]);
}

void eprn(int *R, int i)
{
    printf("+++ Standalone expression evaluates to %d\n", R[i]);
}