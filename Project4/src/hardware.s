.section .text, "ax"
.global OutputString, MoveCursor, ReadController, InitializeTimer, GetTimerTicks
.text
.globl ScreenOffset,TimerTicks
.section .sdata,"aw"
.align 2
.type ScreenOffset, @object
.size ScreenOffset, 4
ScreenOffset:
    .word 0
    .type TimerTicks, @object
    .size TimerTicks, 4
TimerTicks:
    .word 0
    .text
    .text
    .align 2
OutputString:
    # Your code here
    la      a1, 0x500FE800
    lui     a5,%hi(ScreenOffset)
    lw      a3,%lo(ScreenOffset)(a5)
    add     a1,a1,a3
    j       OSL2
OSL1:
    sb      a2,0(a1)
    addi    a0,a0,1
    addi    a1,a1,1
OSL2: 
    lb      a2,0(a0)
    bnez    a2,OSL1
    sw      a3,%lo(ScreenOffset)(a5)
    ret

MoveCursor:
    # Your code here
    slli    a1,a1,6
    add     a0,a0,a1
    lui     a5,%hi(ScreenOffset)
    sw      a0,%lo(ScreenOffset)(a5)
    ret

ReadController:
    # Your code here
    li      a1,0x40000018
    lb      a0,0(a1)
    ret

InitializeTimer:
    # Your code here
    li      a5,0x40000040
    li      a4,0x000186a0 # 0x186a0 is equal to 100000 in decimal  
    lw      a5,0(a5)
    divu    a4,a4,a5
    li      a1,0x40000010
    sw      a4,0(a1)    # Store the value of 100000/value to the cmp
    sw      zero,4(a1)  # Intialize the upper part to zeros
    li      a3, 0x0080  # Get the 8th bits in mie register to 1
    csrw    mie,a3
    li      a3, 0x0008  # Get the 4th bits in mstatus register to 1
    csrw    mstatus,a3
    ret

GetTimerTicks:
    # Your code hre
    lui     a5,%hi(TimerTicks)
    lw      a0,%lo(TimerTicks)(a5)
    ret