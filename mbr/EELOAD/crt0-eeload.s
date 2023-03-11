# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright (c) 2001-2007 ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# Modified startup file for MBR EELOAD
# Removed libc initialization, support for C++ and kernel patches.

   .extern   _heap_size
   .extern   _stack
   .extern   _stack_size

   .extern Exit
   .extern FlushCache

   .set   noat
   .set   noreorder

   .section .start
   .align   2

   .globl  _start
   .ent    _start
_start:

zerobss:
   # clear bss area

   la   $2, _fbss
   la   $3, _end

1:
   sltu   $1, $2, $3
   beq   $1, $0, 2f
   nop
   sq   $0, ($2)
   addiu   $2, $2, 16
   j   1b
   nop
2:

setupthread:
   # setup current thread

   la   $4, _gp
   la   $5, _stack
   la   $6, _stack_size
   la   $7, _args
   la   $8, _root
   move   $gp, $4
   addiu   $3, $0, 60
   syscall         # SetupThread(_gp, _stack, _stack_size, _args, _root)
   move   $sp, $2

   # initialize heap

   la   $4, _end
   la   $5, _heap_size
   addiu   $3, $0, 61
   syscall         # SetupHeap(_end, _heap_size)

   # writeback data cache
   jal FlushCache  # FlushCache(0)
   move   $4, $0

   # call main
   ei

   la   $16, _args
   lw   $4, ($16)
   jal   main      # main(argc, argv)
   addiu   $5, $16, 4

   # call _exit

   j   Exit      # Exit(retval) (noreturn)
   move   $4, $2
   .end   _start

   .align   3

   .ent   _root
_root:
   addiu   $3, $0, 35
   syscall         # ExitThread() (noreturn)
   .end   _root

   .bss
   .align   6
_args:
   .space   4+16*4+256   # argc, 16 arguments, 256 bytes payload
