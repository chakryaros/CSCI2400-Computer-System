#include <iostream>
#include <stdio.h>

using namespace std;


int anyOddBit(int x) {
    int byte = 0xAA;
    int shiftByte = byte + (byte << 8);
    int addbit = shiftByte + ( shiftByte << 16);
    return !!(x & addbit);
}
int allEvenBits(int x) { 
    //Mask=01010101 where all even positions are 1
    //shift it over by 2 bytes and add it again to make it 32 bits long
    //x&masker to get where the x and mask match up (mask only on even positions)
    //Xor so if it's all the same, you get 0 which gets flipped by the bang to 1
  int masker= (0x55 << 8) + 0x55;
  printf("masker 8: %x\n", masker);
  masker = (masker << 16) + masker;
  printf("masker 16: %x\n", masker);
  printf("masker & x: %x\n", (x & masker));
  printf("(x & masker)^masker : %x\n", (x & masker)^masker);

  return !((x & masker)^masker);
}
int addOK(int x, int y) { 
    /*
     * boolean operation show the sign of value
     * 1 is negative
     * 0 is positive
     * x shift to right 31, -0x1
     * y shift to right 31, -0x1
     * sum -0x1 >> 31 equal -0x1
     * x1 ^ y1 = 0x1
     * if x and y is the same sign and sum of x and y is different sign, it's overflow
     */
    int x1 = x >> 31;
    printf("x shiftByte: %d\n", x1);
    int y1 = y >> 31;
    printf("y shiftByte: %d\n", y1);
    int sum = (x+y) >> 31;
    printf("sum: %d\n", sum);
    printf("y1 ^ sum %d\n", y1^sum);
     printf("x1 ^ y1 %d\n", x1^y1);
     printf("~(x1 ^ y1) %d\n", ~(x1^y1));
     printf("~(x1 ^ y1) & ( y1 ^ sum) %d\n", ~(x1 ^ y1) & ( y1 ^ sum));
    
  return !(~(x1 ^ y1) & ( y1 ^ sum)) ;

}

int isAsciiDigit(int x) { 
  //Initialize the sign integer and the limits
  //If >0x39 and <0x30 are added, we get a negative value
  //Adds x and checks for signs, returns whether its in range or not
  //Negative value is not in range, Positive is in range
    int negative= 1<<31;
    printf("negative %d\n", negative);
    int lessthan= ~(negative | 0x39);
    printf("lessthan %d\n", lessthan);
    int greatthan= ~(0x30);
    printf("greatthan %d\n", greatthan);
    
    lessthan = negative & (lessthan+x) >> 31;
    printf("lessthan %x\n", lessthan);
    greatthan= negative & (greatthan+1+x) >>31;
     printf("greatthan %x\n", greatthan);
     printf("return %x\n", !(lessthan | greatthan));
    return !(lessthan|greatthan);
}

int reverseBits(int x) {
    /*
     * Isolate each 8 bits and then switch them to the other side.
     * 
     */
    int mask = 0xff;
    // x = ((x >>1) & 0x55555555) | ((x << 1) & 0xaaaaaaa);
    // x = ((x >> 2) & 0x33333333) | ((x << 3) & 0xcccccccc);
    // x = ((x >> 4) & 0x0f0f0f0f) | ((x << 4) & 0xf0f0f0f0);
    // x = ((x >> 8) & 0x00ff00ff) | ((x << 8) & 0xff00ff00);
    // x = ((x >> 16) & 0x0000ffff) | ((x << 16) & 0xffff0000);
  return ((x >> 24) & mask) | ((x>>8) &(mask << 8)) | ((x << 8) & (mask << 16)) | (x<<24);

}
int main()
{
	// printf("%d\n",anyOddBit(0xaa));
	 //printf("!! %d\n", a);

	 int x = 0x77777777;
	 int y = 0x55555555;
     int b = 0x4;
     printf("%d\n", ~b);
	 //printf("addOk %d\n", addOK(x, y));
	//printf("Odd bit is %d\n", result);
     //isAsciiDigit(0x35);
     //printf("%d\n",reverseBits(0x11111111));
     //printf(" 0xFFFFFFFF:%d\n", allEvenBits(x));
     //printf("0x55555555:%d\n", allEvenBits(y));
}








