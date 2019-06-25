//--------------------------------------------------------------
// 
//  4190.308 Computer Architecture (Spring 2019)
//
//  Project #1: Converting Fixed-Point to Floating-Point
//
//  March 20, 2019
//
//  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//--------------------------------------------------------------


#include <stdio.h>
#include "pa1.h"


// fix2double() returns the double-precision floating-point value for 
// the given fixed-point value 'a'. Note that 'a' is represented in 
// 21.10 fixed-point format.

double fix2double (fixp a)
{
	double fixedPointNum = 0.0;
	if(a<0){
		fixedPointNum = -1.0 * (((double) ~a+1.0))/(1024);
	}
	else{
		fixedPointNum = 1.0*(((double)(a))/(1024));
	}
	return fixedPointNum;
}
