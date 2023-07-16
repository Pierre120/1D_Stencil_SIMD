; Name: HERNANDEZ, Pierre Vincent C.		CEPARCO - S11
section .text
bits 64
default rel
global stencil_1D_x86_64_SIMD

stencil_1D_x86_64_SIMD:
; number of times to loop
shr rcx, 2 ; ymmx (256-bit) / uint (64-bit) == 4 elements will be processed at the same time -> total elem / 4
sub rcx, 1 ; max: n - 1 --> last set of 4 elements will cause an out-of-bounds
; starting index of 2nd param - Y vector
add rdx, 24 ; 3rd index (8 bytes * 3)

; perform stencil
Lsum:
	; move values to registers
	vmovdqu ymm1, [r8] ; ymm1 <--- X[i-3]
	vmovdqu ymm2, [r8 + 8] ; ymm2 <---- X[i-2]
	vmovdqu ymm3, [r8 + 16] ; ymm3 <---- X[i-1]
	vmovdqu ymm4, [r8 + 24] ; ymm4 <---- X[i]
	vmovdqu ymm5, [r8 + 32] ; ymm5 <---- X[i+1]
	vmovdqu ymm6, [r8 + 40] ; ymm6 <---- X[i+2]
	vmovdqu ymm7, [r8 + 48] ; ymm7 <---- X[i+3]
	vpaddq	ymm1, ymm1, ymm2 ; ymm1 <--- X[i-3] + X[i-2]
	vpaddq	ymm1, ymm1, ymm3 ; ymm1 <--- ... + X[i-1]
	vpaddq	ymm1, ymm1, ymm4 ; ymm1 <--- ... + X[i]
	vpaddq	ymm1, ymm1, ymm5 ; ymm1 <--- ... + X[i+1]
	vpaddq	ymm1, ymm1, ymm6 ; ymm1 <--- ... + X[i+2]
	vpaddq	ymm1, ymm1, ymm7 ; ymm1 <--- ... + X[i+3]
	vmovdqu [rdx], ymm1 ; store 4 results in Y vector
	add r8, 32 ; 4 elements * 8 bytes
	add rdx, 32 ; 4 elements * 8 bytes
	loop Lsum

ret