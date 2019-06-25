#--------------------------------------------------------------
#
#  4190.308 Computer Architecture (Spring 2019)
#
#  Project #3: Drawing diagonal lines in an image
#
#  April 24, 2019.
#
#  Jin-Soo Kim (jinsoo.kim@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#--------------------------------------------------------------
.text
	.align 4
.globl bmp_diag
	.type bmp_diag,@function
bmp_diag:
	#------------------------------------------------------------
	# Use %rax, %rcx, %rdx, %rsi, %rdi, and %r8 registers only
	#	imgptr is in %rdi
	#	width  is in %rsi
	#	height is in %rdx
	#	gap    is in %rcx
	#------------------------------------------------------------
		jmp	.L2
	.L3: 											#		marking
		andw	$0, (%rdi)				#		 G, B <- %rdi&0x00
		orb		$-1, 2(%rdi)			# 	 R <- %rdi|0xff
	.L4: 											#		a++, %rdi += 3
		addq	$3, %rdi
		popq	%r8
		incq	%r8
	.L10: 										#  	width>a에서 반복문, %r8=a
		cmpq	%rsi, %r8
		je	.L12
		pushq %r8
		movq	%rdx, %rax
	.L14:											#		gap만큼 빼주면서 rax 음수로 만들기 (%연산 대체)
		subq	%rcx, %rax
		jg	.L14
	.L15:											#		gap만큼 빼주면서 r8 음수로 만들기 (%연산 대체)
		subq	%rcx, %r8
		jg	.L15
	.L16:											#		rax, r8 비교해서 같거나
		cmpq	%rax, %r8					#		두 개 더한 값과 gap의 합이 같으면 mark
		je	.L3									#		아니면 a++
		addq	%rcx, %r8
		addq	%r8, %rax
		je	.L3
		jmp	.L4
	.L12:											#		pad 더해주기
		andq	$3, %r8
		addq	%r8, %rdi
	.L2: 											#		rdx 0이상인 구간 loop, a=0 초기화
		xorq	%r8, %r8
		decq	%rdx
		jns	.L10
	ret
