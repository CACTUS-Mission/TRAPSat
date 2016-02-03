#include <stdio.h>

int main()
{
    int i = 0;
    for(i = 0; i < 54; i++)
    {
        printf("%d / 10 = %d;\t %d % 10 = %d \n", i, (i/10), i, (i%10));     
    }
    return 0;
}
