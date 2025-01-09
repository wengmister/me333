# HW1 ME333 Winter 2025
Author: Zhengyang Kris Weng

### 18. Invoking the gcc compiler with a command like gcc myprog.c -o myprog actually initiates four steps. What are the four steps called, and what is the output of each step?

The four steps are preprocessing, compiling, assemblying, and linking:  
Preprocessing:

    Processes directives like #include, #define, #ifdef, etc.
    Output: A translation unit that includes expanded macros, included files, and removed comments.

Compiling:

    Converts the preprocessed code into assembly code, checking for syntax and semantic errors.
    Output: Assembly code (.s or .asm file), which is platform-specific but not yet machine code.

Assembling:

    Translates assembly code into machine code (object code), which is in binary format but not fully linked.
    Output: Object file (.o or .obj), containing machine code and unresolved references to external symbols.

Linking:

    Combines object files and libraries into a single executable or shared library, resolving symbol references.
    Output: Executable file (e.g., .exe, .out, or binary) or shared library (e.g., .so, .dll), ready to run.

### 19. What is main ’s return type, and what is the meaning of its return value?
Int. The return value is used to indicate the status of the program when it finishes. A return value of 0 generally indicates successful execution.  

A non-zero return value typically represents an error, where the value often serves as an error code to provide more specific information about the failure. Different values may correspond to different types of errors depending on the program's convention.

### 21.Consider three unsigned chars, i , j , and k , with values 60, 80, and 200, respectively. Let sum also be an unsigned char. For each of the following, give the value of sum after performing the addition. (a) sum = i+j; (b) sum = i+k; (c) sum = j+k


### 22. For the variables defined as int a=2, b=3, c; float d=1.0, e=3.5, f; give the values of the following expressions. (a) f = a/b; (b) f = ((float) a)/b; (c) f = (float) (a/b); (d) c = e/d; (e) c = (int) (e/d); (f) f = ((int) e)/d; 

### 27. You have written a large program with many functions. Your program compiles without errors, but when you run the program with input for which you know the correct output, you discover that your program returns the wrong result. What do you do next? Describe your systematic strategy for debugging.

### 28. Erase all the comments in invest.c , recompile, and run the program to make sure it still functions correctly. You should be able to recognize what is a comment and what is not. Turn in your modified invest.c code.

### 30. Consider this array definition and initialization: int x[4] = {4, 3, 2, 1}; For each of the following, give the value or write “error/unknown” if the compiler will generate an error or the value is unknown. (a) x[1] (b) *x (c) *(x+2) (d) (*x)+2 (e) *x[3] (f) x[4] (g) *(&(x[1]) + 1)

### 31. For the (strange) code below, what is the final value of i ? Explain why. int i,k=6; i = 3*(5>1) + (k=2) + (k==6);

### 32. As the code below is executed, give the value of c in hex at the seven break points indicated, (a)-(g).
unsigned char a=0x0D, b=0x03, c;
c = ˜a; // (a)
c = a & b; // (b)
c = a | b; // (c)
c = a ˆ b; // (d)
c = a >> 3; // (e)
c = a << 3; // (f)
c &= b; // (g)

### 34. Write a program to generate the ASCII table for values 33 to 127. The output should be two columns: the left side with the number and the right side with the corresponding character. Turn in your code and the output of the program.

### 35. We will write a simple bubble sort program to sort a string of text in ascending order according to the ASCII table values of the characters.