fn main ()
{
    y = 3 - 1;

    x = factorial(y);

    ret x;
}

fn factorial (a)
{
    if (a == 0)
    {
        ret 1;
    }

    ret a * factorial(y);
}