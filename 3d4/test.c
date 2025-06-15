#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    clock_t start = clock();
    int a = 0;
    for (int i = 0; i < 100000000; i++)
    {
        a++;
    }
    printf("%f\n", (double)(clock()-start)/1000);
    return 0;
}