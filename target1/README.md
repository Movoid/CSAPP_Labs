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
0               7  
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

