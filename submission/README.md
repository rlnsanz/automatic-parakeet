# Project 3

Part 1:
get_addr.c implements a new system call that outputs the physical memory address the virtual mrmory address is mapped to or prints the swap identifier.
The output can be viewed by the command dmesg. The output is similiar to readmap2's. 

This new system call can be tested by compiling and running the user-space program get_addr_user.c. 

## Compilation

Part 1: 
Compile the user-program driver get_addr_user.c as

gcc get_addr_user.c -o get_addr_user

and run it with the following command

sudo ./get_addr_user [PID] [VIRTUAL ADDRESS]

[PID] is the PID in decimal
[VIRTUAL ADDRESS] is the hex address of the virtual page without the 0x prefix


## Grading Considerations

The user space program must be run with super user privileges.

The output of the program should be viewed with the dmesg commane.

## Credits

Daniel D'Souza,
Rolando Garcia,
and James Hutchins
