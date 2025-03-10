# ./ctarget 

ret addr hack in 0x00000000004017AF

## Phase 1

ret to touch1.

## Phase 2

two resolutions.

ROP Gadget, use `pop %rdi; ret;` to set `%rdi`,
then ret to touch2.

![phase2_trick](./phase2_trick.png)

or construct `pop %rdi; ret;` in stack.


## Phase 3

need to let string of `*%rdi` == cookie (sprintf transdered it to ASCII) .  
construct ASCII string in stack, and let %rdi point to it (use ROPGadget in Phase 2).  

`hexmatch` in `touch3` will compare string which is `%rdi` pointed to with the *random addressed* ASCII string of cookie in `touch3` stack.  


```
hacked stack:  
0                7  
+----------------+  
| ASCII STR      |    
+----------------+
| touch3 addr    |  
+----------------+  
| HACKED %rdi    |
| point to ASCII |
+----------------+  
| ROPGadget addr |
+----------------+  
|                |
.                .
.  trash bytes   .
.                .
|                |
+----------------+ <- %rsp
```


# ./rtarget

## Phase 4

same to Phase1.
but you will get a SIGSEGV in `ld.so` cuz,  

```asm
libc.so.6:00007FFFF7E19F9A movaps  xmmword ptr [rbp-40h], xmm0
```

have to align `%rbp` as 16 bytes to use `xmm` registers.  

...

noticed that `%rbp` is changed by,

```asm
libc.so.6:00007FFFF7EBD630 mov     rbp, rsp
```

then you have to align `%rsp` as 16 bytes before hacked to `touch1` .

there is a simple way, just avoid `touch1`'s `sub rsp, 8`. it might corrupt the stack but is able to pass Phase 4.  

## Phase 5

same to Phase 4, use ROP Gadget in Phase 2,  
but avoid `sub rsp, 8` in `touch2` .

## Phase 6

there is no `sub` but `push` in `touch3`.  

use `ret` to adjust `%rsp` (+8) to align as 16 bytes.

> noticed that `rtarget` used Stack ASLR, which prevented from using absolute vaddr as in the Phase 3 solution.

still use gadgets,

> it's kinda hard to think how to contruct gadgets.  

