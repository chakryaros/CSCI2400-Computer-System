/* 
 * CS:APP Data Lab 
 * 
 * <chakrya ros 107104368>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
    /*
     * De Morgan's law
     */
  return ~(~x & ~y);
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) {
    /* store 0x55 in mask
     * 0x55 is evenbit as 0101 0101 binary
     * store byte plus byte by shifting by 8 in shiftByte (0x5555)
     * store shiftByte plue shiftByte by shifting by 16 in word (0x55555555)
     * to get 32 bit long
     */
    int mask = 0x55;
    int shiftByte = mask + (mask << 8);
    int word = shiftByte + (shiftByte << 16);
  return word;
}
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
    /*
     * 00000 give the complement to 11111 that return -1
     */
    int x = 0x00;
  return ~x;
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
    /*
     * Mask=01010101 mean all even positions are 0
     * shiftByte equalt to Mask plus Mask by shifting by 8, evenbit become 0x5555 
     * evenbit equalt to shiftbyte plus shiftbyte by shifting
     * by 16, eventbit become 0x555555 to get 32 bit long
     * x and evenbit to get where the x and evenbit match up (evenbit only on even positions)
	 * Xor so if it's all the same, you get 0 which gets flipped by the bang to 1
     */
    int mask = 0x55;
    int shiftByte = mask + (mask << 8);
    int evenbit = shiftByte + (shiftByte << 16);
  return !((x & evenbit) ^ evenbit);
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) {
    /*
     * Mask = 1010 1010 mean all odd positions are 0
     * shift 8, evenbit become 0xAAAA, 16bits
     * shift 16, eventbit become 0xAAAAA, 32bits
     * Double !! operation means result from bit string to boolean value
     */
    int byte = 0xAA;
    int shiftByte = byte + (byte << 8);
    int oddbit = shiftByte + ( shiftByte << 16);
    return !!(x & oddbit);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
    /*
     * shifting n and m positon by 3 like n and m multiply by 3
     * n = 1 and shift left 3 equal to n = 8 (1000),
     * m = 3 and shift left 3 equalt to m = 24 (0001 1000)
     * use specific bit to remove byte from x and to store byte that will be move
     * add all up
     * x_N mean X shift to right by 8 and multiply by 0xff
     * 
     */
    int n_shift = n << 3;
    int m_shift = m << 3;
    int n_pos = ~(0xff << n_shift);     //0xff shift to left by 8 and complement
    int m_pos = ~(0xff << m_shift);     //oxff shift to left by 24 and complement
    int x_N = (x >> n_shift) & 0xff;    //x shift to right by 8 and match up x & 0xff
    int x_m = ( x >> m_shift) & 0xff;   //x shift to right by 24
    int xMask = x & (n_pos & m_pos);
    return xMask | ((x_N << m_shift) | x_m << n_shift);
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
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
    int y1 = y >> 31;
    int sum = (x+y) >> 31;
  return !(~(x1 ^ y1) & ( y1 ^ sum)) ;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    /*
     * !x = 0, ~0x00 = 0xff
     * m = 0xff (1111 1111) 
     * y = 0100
     * (y & m) = 0000 0100 (0x04) 
     * z = 0101, ~m = 0000 0000 
     * (z & ~m) = 0000 0000
     * return 4
     */
    int m = ~0x00 + !x;
  return (y & m) | (z & ~m) ;
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
     /* store x (whole byte = 8 bit) in x_shift3 by shifting by 3
      * store x in x_shift1 by shifting by 1
      * (x >> 3) ^ 0x6 returns 0 for 0xc0 through 0xc7 and 
      * (x >> 1) ^ 0x1c will only return 0 for 0xc8 and 0xc9
      * !(0x30 <= x <=0x37) >> 3 and XOR 0x6 become 1
      * !(0x38 <= x <=0x39) >> 3 and XOR 0x6 become 0
      */
    int x_shift3 = x >> 3;
    int x_shift1 = x >> 1;
  return !(x_shift3 ^ (0x6)) | !(x_shift1 ^ (0x1c));
}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
    /* 
      * Store whole byte n in nShift by shifting by 3
	  * Mask of 1's that shifts by a byte
	  * Shift bytes and replace with the byte c that you wanted to add
      * by using mask and OR 
      * 1 << 3 equal to 1000 (0x8)
      * (0xFF << nShift) equal to 0xFF00 (1111 1111 0000 0000)
      *  complement to 0000 0000 1111 1111
      */
    int nShift = n << 3;
    int xMask = x & ~(0xFF << nShift);
  return (xMask) | ( c << nShift);
}
/* reverseBits - reverse the bits in a 32-bit integer,
              i.e. b0 swaps with b31, b1 with b30, etc
 *  Examples: reverseBits(0x11111111) = 0x88888888
 *            reverseBits(0xdeadbeef) = 0xf77db57b
 *            reverseBits(0x88888888) = 0x11111111
 *            reverseBits(0)  = 0
 *            reverseBits(-1) = -1
 *            reverseBits(0x9) = 0x90000000
 *  Legal ops: ! ~ & ^ | + << >> and unsigned int type
 *  Max ops: 90
 *  Rating: 4
 */
int reverseBits(int x) {
    /*
     * store x in x by shifting to right by 1 and match up to 0x55555555u and switch the shift to left by 1
     * store x in x by shifting to right by 2 and match up to 0x33333333u and switch the shift to left by 2
     * store x in x by shifting to right by 4 and match up to 0x0f0f0f0fu and switch the shift to left by 4
     * store x in x by shifting to right by 8 and match up to 0x00ff00ffu and switch the shift to left by 8
     * store x in x by shifting to right by 16 and match up to 0x0000ffffu and switch the shift to left by 16
     */
      x = ((x >>1) & 0x55555555u) | ((x & 0x55555555u) << 1);
      x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
      x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu)  << 4);
      x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
      x = ((x >> 16) & 0x0000ffffu) | ((x & 0x0000ffffu) << 16);
  return x;

}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
    int sum = x + y;
    int overflow = ((x ^ sum) &(y ^ sum)) >> 31;
  return (sum >> (overflow & 31)) + (overflow << 31);
}
/*
 * Extra credit
 */
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
    
     /*
     * 0x7ffffffff is unsigned of max positive of 32 bit long
     */
    unsigned m = 0x7fffffff;
    unsigned min_Nan = 0x7f800001;
    unsigned result = uf & m;
    if(result >= min_Nan)
    {
        return uf;
    }
    else
    {
        return result;
    }
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    return 2;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
  return 2;
}
