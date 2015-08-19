	.file	"matrix_storage.cpp"
	.section	.text.unlikely,"ax",@progbits
	.align 2
.LCOLDB0:
	.text
.LHOTB0:
	.align 2
	.p2align 4,,15
	.globl	_ZNK14SnowflakeModel13MatrixStorage10sumComputeEv
	.type	_ZNK14SnowflakeModel13MatrixStorage10sumComputeEv, @function
_ZNK14SnowflakeModel13MatrixStorage10sumComputeEv:
.LFB764:
	.cfi_startproc
	movq	8(%rdi), %rax
	vxorpd	%xmm0, %xmm0, %xmm0
	movq	16(%rdi), %rdx
	cmpq	%rax, %rdx
	je	.L2
	.p2align 4,,10
	.p2align 3
.L3:
	vaddpd	(%rax), %ymm0, %ymm0
	addq	$32, %rax
	cmpq	%rax, %rdx
	jne	.L3
.L2:
	andl	$-2, 40(%rdi)
	vhaddpd	%xmm0, %xmm0, %xmm1
	vextractf128	$0x1, %ymm0, %xmm0
	vaddsd	%xmm0, %xmm1, %xmm1
	vunpckhpd	%xmm0, %xmm0, %xmm0
	vaddsd	%xmm0, %xmm1, %xmm0
	vmovsd	%xmm0, 32(%rdi)
	vzeroupper
	ret
	.cfi_endproc
.LFE764:
	.size	_ZNK14SnowflakeModel13MatrixStorage10sumComputeEv, .-_ZNK14SnowflakeModel13MatrixStorage10sumComputeEv
	.section	.text.unlikely
.LCOLDE0:
	.text
.LHOTE0:
	.ident	"GCC: (Ubuntu 5.1.0-0ubuntu11~14.04.1) 5.1.0"
	.section	.note.GNU-stack,"",@progbits
