section .text
bits 64
default rel
global stencil_1D_x86_64

stencil_1D_x86_64:
; number of times to loop
sub rcx, 6 ; max: n - 6
; starting index of 2nd param - Y vector
add rdx, 24 ; 3rd index (8 bytes * 3)

; perform stencil
Lsum:
	mov rbx, [r8] ; rbx <--- X[i-3]
	add rbx, [r8 + 8] ; rbx <--- X[i-3] + X[i-2]
	add rbx, [r8 + 16] ; rbx <--- ... + X[i-1]
	add rbx, [r8 + 24] ; rbx <--- ... + X[i]
	add rbx, [r8 + 32] ; rbx <--- ... + X[i + 1]
	add rbx, [r8 + 40] ; rbx <--- ... + X[i  + 2]
	add rbx, [r8 + 48] ; rbx <--- ... + X[i + 3]
	mov [rdx], rbx ; store result to output vector Y
	add r8, 8 ; move to next starting index
	add rdx, 8 ; move to the next index in Y vector
	loop Lsum

ret
