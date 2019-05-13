# include<stdio.h>

int f(int x)
{
   printf("x is %d\n", x);
   return x;
}

int foo(unsigned char *cp, signed int *p)
{
    return *cp + *p; // cc $s load-example.c to see assembly code
                    // cc $c load-example.c
}

int main () {

    // float a = 132.4; 

    // int b = (int) a; 

    // printf("%d\n", b);

    // a = 139.9; 

    // int c = (int) a; 

    //printf("%d\n", c);

    // if ( f(0) && f(1) ) {
    //    printf("Got it!\n");
    // } else {
    //    printf("Done!\n");
    // }

    // if ( f(0) || f(1) ) {
    //    printf("Got it!\n");
    // } else {
    //    printf("Done!\n");
    // }

    // if ( f(1) || f(0) ) {
    //    printf("Got it!\n");
    // } else {
    //    printf("Done!\n");
    // }

    int a =10;

    printf("Please input an integer values: ");
    scanf("%d", &a);
    printf("You entered: %d\n", a);



    return 0;

}