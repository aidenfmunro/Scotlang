fn main ()
{
    y = 10;

    x = factorial(y);

    print(x);
}

fn factorial (a)
{
    if (a == 0)
    {
        ret 1; 
    }

    ret a * factorial(a - 1);
}


char buffer[100];

typedef int (* execBuf)()

execBuf bufexe = *(execBuf *) buffer

bufexe();

JIT

/ x = 5 + 5 /

push [rax + 2]
push 5
add
pop [rax + 3]

-------

push []
push 5

pop rax
pop rbx
add rax, rbx 
push rax

pop rax
mov [r15], rax

-------

---------------------------

func1:

push 3 

pop [rax] ; d

push 2

pop [rax + 1] ; c

---------------------------

func2:

push 2

pop [rax]; a          [][]

push 0

pop [rax + 1]; b

add rax, 2

call func1

push [rax]
push 1
add

----------------------

PROBLEM:

add rax, 2
call func1

ret 

EXAMPLE:

func1 -> rax = 10

a: 10 
b: 10 + 1 = 11

--------------------\
add rax, 2          |
call func2          | <== solution | stack frame add <=> push rbp ; mov rbp, rsp ... pop rbp
sub rax, 2          |
--------------------/


func 1 ()

a = 
b = 

if (...)
{
    c =
    d 
}

add rax, 2

jmp

c: [rax]

d: [rax + 2]

sub rax, 2



c: 12
d: 13
e: 14

sub 2
ret 
---------------------------

add

pop [...]

push [...] 

push [...]

push 1

sub

call factorial

mul

ret

push a - 3
...

-> push a

y: [1000]

x: [1001]

a: [1002] 