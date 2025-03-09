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
