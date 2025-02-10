# Homework 6 ME333 - Winter 2025
Zhengyang Kris Weng Submission 02/10/2025

## Chapter 5.

### 3. Build the program with no optimization and look at the disassembly. Answer the following questions.
#### a. Which combinations of data types and arithmetic functions result in a jump to a subroutine? From your disassembly file, copy the C statement and the assembly commands it expands to (including the jump) for one example. 

        j3 = j1/j2;
    9d0086bc:	8fc60028 	lw	a2,40(s8)
    9d0086c0:	8fc7002c 	lw	a3,44(s8)
    9d0086c4:	8fc40020 	lw	a0,32(s8)
    9d0086c8:	8fc50024 	lw	a1,36(s8)
    9d0086cc:	0f401e52 	jal	9d007948 <__divdi3>   <----------- Jumps here
    9d0086d0:	00000000 	nop
    9d0086d4:	afc20050 	sw	v0,80(s8)
    9d0086d8:	afc30054 	sw	v1,84(s8)

        9d007948 <__divdi3>:   <---------------------------------- Jumps to
    9d007948:	04a10007 	bgez	a1,9d007968 <__divdi3+0x20>
    9d00794c:	00005025 	move	t2,zero
    9d007950:	00041023 	negu	v0,a0
    9d007954:	0002182b 	sltu	v1,zero,v0
 
    ... omitting 200 lines here...

    9d007d78:	1080ff3e 	beqz	a0,9d007a74 <__divdi3+0x12c>
    9d007d7c:	00000000 	nop
    9d007d80:	2442ffff 	addiu	v0,v0,-1
    9d007d84:	1000ff3b 	b	9d007a74 <__divdi3+0x12c>
    9d007d88:	00001825 	move	v1,zero


#### b. For those statements that do not result in a jump to a subroutine, which combination(s) of data types and arithmetic functions result in the fewest assembly commands? From your disassembly, copy the C statement and its assembly commands for one of these examples. Is the smallest data type, char , involved in it? If not, what is the purpose of extra assembly command(s) for the char data type vs. the int data type?

        i3 = i1+i2;
    9d0085d0:	8fc30014 	lw	v1,20(s8)
    9d0085d4:	8fc20018 	lw	v0,24(s8)
    9d0085d8:	00621021 	addu	v0,v1,v0
    9d0085dc:	afc2004c 	sw	v0,76(s8)

The `int` data type takes the fewest assembly commands. Extra steps in the `char` data type is doing memory operations since the `char` is shorter, and it takes a step to mask to only keep the lower 8 bits for the operation.


#### c. Fill in the following table. Each cell should have two numbers: the number of assembly commands for the specified operation and data type, and the ratio of this number (greater than or equal to 1.0) to the smallest number of assembly commands in the table.

              char         int         long long       float     long double
        +   1.25 (5)     1.0 (4)     2.75 (11)          J            J
        -   1.25 (5)     1.0 (4)     2.75 (11)          J            J
        *   1.25 (5)     1.0 (4)     4.50 (18)          J            J
        /   1.75 (7)     1.75 (7)          J            J            J


#### d. From the disassembly, find out the name of any math subroutine that has been added to your assembly code, and find their mappings in .map file.

This part of the disassembly file contains, and they map to:

    __addsf3 --> 0x000000009d008a9c
    __subsf3 --> 0x000000009d008a94
    __mulsf3 --> 0x000000009d008f3c
    __divsf3 --> 0x000000009d008d0c

    __divdi3 --> 0x000000009d007948
    
    __adddf3 --> 0x000000009d007d94
    __subdf3 --> 0x000000009d007d8c
    __muldf3 --> 0x000000009d0081bc
    __divdf3 --> 0x000000009d007490


### 4. Let us look at the assembly code for bit manipulation. How many commands does each use? For unsigned integers, bit-shifting left and right make for computationally efficient multiplies and divides, respectively, by powers of 2.

        u3 = u1 & u2; // bitwise AND    <------ 4 commands
    9d0078d0:	8fc30000 	lw	v1,0(s8)
    9d0078d4:	8fc20004 	lw	v0,4(s8)
    9d0078d8:	00621024 	and	v0,v1,v0
    9d0078dc:	afc20008 	sw	v0,8(s8)
        u3 = u1 | u2; // bitwise OR     <------ 4 commands
    9d0078e0:	8fc30000 	lw	v1,0(s8)
    9d0078e4:	8fc20004 	lw	v0,4(s8)
    9d0078e8:	00621025 	or	v0,v1,v0
    9d0078ec:	afc20008 	sw	v0,8(s8)
        u3 = u2 << 4; // shift left 4 spaces, or multiply by 2ˆ4 = 16   <------ 3 commands
    9d0078f0:	8fc20004 	lw	v0,4(s8)
    9d0078f4:	00021100 	sll	v0,v0,0x4
    9d0078f8:	afc20008 	sw	v0,8(s8)
        u3 = u1 >> 3; // shift right 3 spaces, or divide by 2ˆ3 = 8     <------ 3 commands
    9d0078fc:	8fc20000 	lw	v0,0(s8)
    9d007900:	000210c2 	srl	v0,v0,0x3
    9d007904:	afc20008 	sw	v0,8(s8)


however, on the side note... I also did multiplication and division normally but they seem to take the same amount of commands?
        u3 = u2 * 16;       <------ 3 commands
    9d00788c:	8fc20004 	lw	v0,4(s8)
    9d007890:	00021100 	sll	v0,v0,0x4
    9d007894:	afc20008 	sw	v0,8(s8)
        u3 = u2 / 8;        <------ 3 commands
    9d007898:	8fc20004 	lw	v0,4(s8)
    9d00789c:	000210c2 	srl	v0,v0,0x3
    9d0078a0:	afc20008 	sw	v0,8(s8)

Not sure why this is the case ¯\_(ツ)_/¯. I have optimization set at -O0.

## Chapter 6

### 1. 