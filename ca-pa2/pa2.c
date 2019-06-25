//--------------------------------------------------------------
//
//  4190.308 Computer Architecture (Spring 2019)
//
//  Project #2: TinyFP Representation
//
//  April 4, 2019
//
//  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//--------------------------------------------------------------

#include <stdio.h>
#include "pa2.h"
#define minNumF 0.015625
#define mindivtwoF 0.0078125
#define minNumI 16
#define mindivtwoI 8


int twopowercal(int k){
	if(k==0){
		return 1;
	}
	return 2*twopowercal(k-1);
}
float twopowercalF(float k){
	if((k-0.0<0.00001)&&(k-0.0>-0.00001)){
		return 1;
	}
	if(k>0){return 2*twopowercalF(k-1);}
	if(k<0){return twopowercalF(k+1)/2.0;}
}
int twodivcal(int k){
	if(k==1){
		return 0;
	}
	return 1+twodivcal(k/2);
}
float twodivcalF(float k){
	if(k-minNumF<0.000001){
		return 0.0;
	}
	return 1.0+twodivcalF(k/2.0);
}
float findroundPointF(float p, float q){
	if (p/q>=2.0){
		return findroundPointF(p, q*2.0);
	}
	else {
		return q;
	}
}
int findroundPointI(fixp a, int b){
	if((a/b)>=2){
		return findroundPointI(a,b*2);
	}
	else{
		return b;
	}
}
fixp roundingfixp(fixp f){
	int roundPoint = findroundPointI(f,minNumI);
  int gBit = (f&(roundPoint/16)) ? 1 : 0;
  int rBit = (f&(roundPoint)/32) ? 1 : 0;
  int sBit = ((f%((roundPoint)/32))>0) ? 1 : 0;
	if (gBit*rBit == 1 || rBit*sBit == 1){
    f += roundPoint/16;
  }
  int cnt = roundPoint/32;
  while(cnt>=1){
    if(f&cnt){
      f = f - cnt;
    }
    cnt /= 2;
  }
  return f;
}

tinyfp fixp2tinyfp(fixp f){
	if(f<0){
		f = (~f)+1;
		if(f>=0x00003f00){//-infinity
			return 0xf0;
		}
		else if((f<=0x00000008) && (f>=0x00000000)){//-0.0
			return 0x80;
		}
		else if((f<=0x000000ff) && (f>=0x00000009)){ // denormalize form, neg, exp 000

	    int gBit = (f&(0x00000010)) ? 1 : 0;
	    int rBit = (f&(0x00000008)) ? 1 : 0;
	    int sBit = ((f%(0x00000008))>0) ? 1 : 0;
	    if ((gBit*rBit == 1)||(rBit*sBit == 1)){
	      f += 0x00000010;
	    }
	    int cnt = 0x00000008;
	    while(cnt>=1){
	      if(f&cnt){
	        f = f - cnt;
	      }
	      cnt /= 2;
	    }
			for(int i = 1; i < 16; i++){
	      if((f>i*minNumI-mindivtwoI) && (f<i*minNumI+mindivtwoI)){
					return 0x80 + i;
				}
			}
	    return 0x80 + 0x10;
		}
		else{ // normalize form, neg
			f = roundingfixp(f);
			int roundPoint = findroundPointI(f,minNumI);
			for(int i = 0; i < 16; i++){
				if((f>roundPoint+i*roundPoint/16-roundPoint/32) && (f<roundPoint+i*roundPoint/16+roundPoint/32)){
					return 0x80 + 16*(twodivcal(roundPoint)-7) + i;
					}
				}
				return 0x80 + 16*(twodivcal(roundPoint)-7+1);
		}
	}
	else{
		if(f>=0x00003f00){//+infinity
			return 0x70;
		}
		else if((f<=0x00000008) && (f>=0x00000000)){//+0.0
			return 0x00;
		}
		else if((f<=0x000000ff) && (f>=0x00000009)){ // denormalize form, pos, exp 000
	    int gBit = (f&(0x00000010)) ? 1 : 0;
	    int rBit = (f&(0x00000008)) ? 1 : 0;
	    int sBit = ((f%(0x00000008))>0) ? 1 : 0;
	    if ((gBit*rBit == 1)||(rBit*sBit == 1)){
	      f += 0x00000010;
	    }
	    int cnt = 0x00000008;
	    while(cnt>=1){
	      if(f&cnt){
	        f = f - cnt;
	      }
	      cnt /= 2;
	    }
			for(int i = 1; i < 16; i++){
	      if((f>i*minNumI-mindivtwoI) && (f<i*minNumI+mindivtwoI)){
					return i;
				}
			}
	    return 0x10;
		}
		else{ // normalize form, pos
			f = roundingfixp(f);
			int roundPoint = findroundPointI(f,minNumI);
			for(int i = 0; i < 16; i++){
				if((f>roundPoint+i*roundPoint/16-roundPoint/32) && (f<roundPoint+i*roundPoint/16+roundPoint/32)){
					return 16*(twodivcal(roundPoint)-7) + i;
					}
				}
				return 16*(twodivcal(roundPoint)-7+1);
		}
	}
}

fixp tinyfp2fixp(tinyfp t){
	if(((t>=0x70)&&(t<=0x7f))||((t>=0xf0)&&(t<=0xff))){ // smallest number
		return 0x80000000;
	}
	if(t>0x80){
		t = t-0x80;
		if(t<0x10){ // denormalize, neg, exp 000
			return (~(t*0x00000010)+1); // value at pos
		}
		else{ // normalize value, neg
			return (~(((t&0x0f)+0x10)*twopowercal(((t&0x70)/minNumI)+3))+1); // value
		}
	}
	else{
		if((t==0x00)||(t==0x80)){ // 0.0
			return 0x00000000;
		}
		else if(t<0x10){ // denormalize, pos, exp 000
			return t*0x00000010; // value at pos
		}
		else{ // normalize value
			return ((t&0x0f)+0x10)*twopowercal(((t&0x70)/minNumI)+3); // value
		}
	}
}


tinyfp float2tinyfp(float f){
	if(f!=f){//NaN
		int a = *(int*)&f;
		if(a>0){
			return 0x71;
		}
		else{
			return 0xf1;
		}
	}
	else if(f==0.0){ // special case, 0.0 and -0.0
		int a = *(int*)&f;
		if(a==0){
			return 0x00;
		}
		else{
			return 0x80;
		}
	}
	else if(f<0.0){
		f*=-1.0;
		if(f>=15.75){ // -infinity
			return 0xf0;
		}
		else if((f<=mindivtwoF) && (f>=0.0)){ // =0.0
			return 0x80;
		}
		else if((f<0.25) && (f>mindivtwoF)){ // denormalize form, neg;
			for(int i = 0; i < 16; i++){
				if(i%2==1){
					if(f>i*minNumF-mindivtwoF && f<i*minNumF+mindivtwoF){
						return 0x80 + i;
					}
				}
				else{
					if(f>=i*minNumF-mindivtwoF && f<=i*minNumF+mindivtwoF){
						return 0x80 + i;
					}
				}
			}
			return 0x80 + 0x10;
		}
		else{ // normalize form, neg
			float roundPoint = findroundPointF(f, minNumF);
			for(int i = 0; i < 16; i++){
				if(i%2==1){
					if((f>roundPoint+i*roundPoint/16.0-roundPoint/32.0) && (f<roundPoint+i*roundPoint/16.0+roundPoint/32.0)){
						return 0x80 + 16*(((int) twodivcalF(roundPoint))-3) + i;
					}
				}
				else{
					if((f>=roundPoint+i*roundPoint/16.0-roundPoint/32.0) && (f<=roundPoint+i*roundPoint/16.0+roundPoint/32.0)){
						return 0x80 + 16*(((int) twodivcalF(roundPoint))-3) + i;
					}
				}
			}
			return 0x80 + 16*(((int) twodivcalF(roundPoint))-2);
		}
	}
	else{
		if(f>=15.75){
			return 0x70;//+infinity
		}
		else if((f<=mindivtwoF) && (f>=0.0)){ // +0.0
			return 0x00;
		}
		else if((f<0.25) && (f>mindivtwoF)){ // denormalize form, pos
			for(int i = 0; i < 16; i++){
				if(i%2==1){
					if(f>i*minNumF-mindivtwoF && f<i*minNumF+mindivtwoF){
						return i;
					}
				}
				else{
					if(f>=i*minNumF-mindivtwoF && f<=i*minNumF+mindivtwoF){
						return i;
					}
				}
			}
	    return 0x10;
		}
		else{ // normalize form, pos
			float roundPoint = findroundPointF(f, minNumF);
			for(int i = 0; i < 16; i++){
				if(i%2==1){
					if((f>roundPoint+i*roundPoint/16.0-roundPoint/32.0) && (f<roundPoint+i*roundPoint/16.0+roundPoint/32.0)){
						return 16*(((int) twodivcalF(roundPoint))-3) + i;
					}
				}
				else{
					if((f>=roundPoint+i*roundPoint/16.0-roundPoint/32.0) && (f<=roundPoint+i*roundPoint/16.0+roundPoint/32.0)){
						return 16*(((int) twodivcalF(roundPoint))-3) + i;
					}
				}
			}
			return 16*(((int) twodivcalF(roundPoint))-2);
		}
	}
}


float tinyfp2float(tinyfp t){
	if((t>=0x71)&&(t<=0x7f)){ // +Nan
		return (1.0/0.0)*0.0*(-1.0);
	}
	else if((t>=0xf1)&&(t<=0xff)){ // -Nan
		return (1.0/0.0)*0.0*(1.0);
	}
	else if(t==0x00){
		return 0.0; // +0.0
	}
	else if(t==0x80){
		return -0.0; // -0.0
	}
	else if(t==0x70){ // +infinity
		return (1.0/0.0);
	}
	else if(t==0xf0){ // -infinity
		return (1.0/0.0)*(-1.0);
	}
	else if(t<0x10){
		return 1.0*((t&0x0f)/64.0);
	}
	else if((t<0x90)&&(t>0x80)){
		return -1.0*((t&0x0f)/64.0);
	}
	else{
		if(t<0x80){// value at pos
			return 1.0*twopowercalF(((t&0x70)/16)-3)*(((t&0x0f)+0x10)/16.0);
		}
		else{ // value at neg
			return -1.0*twopowercalF(((t&0x70)/16)-3)*(((t&0x0f)+0x10)/16.0);
		}
	}
}
