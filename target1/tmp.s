# this is needed constructions, with gadgets.

# leaq (%rdi, %rsi, 1), %rax

movq %rsp, %rax
ret

movq %rax, %rdi
ret

pop %rsi
ret
