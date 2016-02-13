# Project 2

get_proc_custom.c implements a new system call that lists all the processes in the system. The output is similar in form to ps -e. 

This new system call can be tested by compiling and running the user-space program fake_ps.c. 

## Compilation

Because the user-program driver fake_ps.c includes math.h, compile as

gcc fake_ps.c -o fake_ps.out -lm

## Grading Considerations

It is not possible to pass memory addresses from kernel space into user space: dereferencing a pointer in user-space that is valid in kernel-space will cause a segmentation fault.

To work around this constraint, it is possible to allocate a memory block m1 in user-space, and pass the address to m1 to a system call. Then, the system call can pass data back to the user space by using the m1 block.

The *proc_count and *proc_array formal parameters in sys_get_proc_custom are used for these purposes.

## Credits

Daniel D'Souza,
Rolando Garcia,
and James Hutchins
