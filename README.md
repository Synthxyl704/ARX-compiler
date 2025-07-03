# ARX compiler
Started on 13th June, (at the time) 7 months into programming (in general) - a simple compiler for my language which im learning to make for pushing and increasng my skills in low level understanding and compiler design, completely open source and you can tweak it to your liking, make changes or learn if you want.

<hr>> Project currently under construction, UPDs and rewrites will be updated frequently as needed to respective files.
<hr>

## Run Locally

Clone the project using this command onto bash:
```bash
git clone https://github.com/Synthxyl704/ARX-compiler
```

Linux terminal guide with neovim:
```
-> cd <directory of storage>
-> gcc isomain.c -o isomain
-> ./isomain ./isoreadfile.arx 
```

## Current language documentation for reference

- _ret - return
- _prov - main
- _subprogram - functions
- UC

## ARX "not-so-friendly-but-works" print format for errors and stuff
```
// example: std:syntax:error::[TC3/L1]::[retx6]:::[_ret]
```
TC[X] = TokenCount (in file) at [X] position horizontally wrt L[X]
L[X]  = Line count (in file) at [X] position vertically
: = program standard (intrinsic to (the) program)
:: = file standard (may be intrinsic to both program + the file read)
::: = program standard amelioration tweak string


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

## Epic learningx & tinkering history/remarks

```
#define MAX_TOKENS_USAGE 0x7F6E
```
This was actually completely random, 0x7F6E is 32622, a completely srand(); number.

```
#define MAX_TOKENS_USAGE 0x7F6E
```
## License

[MIT](https://choosealicense.com/licenses/mit/)

<h3>IsoAris -REVITALIZE-</h3>
