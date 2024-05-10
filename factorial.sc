fn main ()
{
    y = 0;

    input (y);

    x = factorial(y);

    print(x);

    ret;
}

fn factorial (a)
{
    if (a == 0)
    {
        ret 1; 
    }

    ret a * factorial(a - 1);
}
