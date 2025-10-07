# ARX compiler
ARX is a "derivative" language inspired from C, C++ and ASM. <br>
Project started on 13th June with mediocre C syntactical knowledge, a fun project I started on for extending my knowledge in low level compiler design. <br>
What makes it stand out than other compilers? | Currently nothing of significance :(
<hr>

> [!NOTE]
> The compiler is still incomplete and has alot of issues in its current state. <br>
> Updates, rewrites and refactorings of the code will be logged appropriately.

<hr>

## Run Locally

Clone the project using this command onto bash:
```bash
git clone https://github.com/Synthxyl704/ARX-compiler
```

[Linux] | Terminal guide:
```
-> cd <directory of storage>
-> gcc isomain.c -o isomain
-> ./isomain ./isoreadfile.arx 
```
# Syntax manual:
## Current language documentation for reference

### All existing datatypes:
| Datatype | Syntax      |      
|----------|--------     |
| Integers     | mv_int  | 
| Floats       | mv_flt  |
| Double       | mv_dbl  |
| Character    | mv_chr  | 

Trivia: `mv` means "moving" in ARX, that means when you type:
```
mv_int 1E5, varX;
```
You are telling ARX to "move this `int`/integer with `value=1E5/100000` into `varX` <br>
Same for flt, chr, and dbl!

### Functions:
In ARX, functions are conventionally known as "subprograms"
| Token | Syntax      |      
|----------|--------     |
| Function     | _subprog  | 

### Main and return:
| Token | Syntax      |      
|----------|--------     |
| Main     | _prov  | 
| Return     | _ret | 

## ARX "not-so-friendly-but-works" print format for errors and stuff
```
// example: std:syntax:error::[TC3/L1]::[retx6]:::[_ret]
```
TC[X] = TokenCount (in file) at [X] position horizontally wrt L[X] <br>
L[X]  = Line count (in file) at [X] position vertically <br>
: = program standard (intrinsic to (the) program) <br>
:: = file standard (may be intrinsic to both program + the file read) <br>
::: = program standard amelioration tweak string <br>


## Learning material for compiler designs

I used these epic peoples' assistance to build so far:
 - [Pixeled's guide to compiler design](https://www.youtube.com/playlist?list=PLUDlas_Zy_qC7c5tCgTMYq2idyyT241qs)
 - [DoctorWkt's acwj](https://github.com/DoctorWkt/acwj/blob/master/01_Scanner/Readme.md)
 - [How to Build a New Programming Language](https://pgrandinetti.github.io/compilers/page/how-to-build-a-new-programming-language/#:~:text=Yes.,to%20you%20the%20machine%20code)


## Definitely not FAQs

#### Why is this still incomplete?

Because at the time I was a 16 year old 1st year student making something that was kind of a bit of a jump leap, but it was fun anyway. Will take quite alot of time to complete at the/my best guess.

#### Why isnt this OOP/or actually usable?

ARX was/is simply created for enhancing my knowledge in compiler design and argument vector/file readings and executions, ASM codegens and stuff, you can always take a look at what I did and leave me some suggestions/constructive-criticisms, if I am alive I will definitely reply to each message, thanks!

#### Why is everything in FAQs in past tense?

Because why not.

## UPD/RR logs:

> N/A

```
#define MAX_TOKENS_USAGE 0x7F6E
```
## License

[MIT](https://choosealicense.com/licenses/mit/)

<h3>IsoAris -REVITALIZE-</h3>
