fn main ()
{
    y = 10;

    x = factorial(y);
}

fn factorial (a)
{
    if (a == 0)
    {
        ret 1; 
    }

    ret a * factorial(a - 1);
}
