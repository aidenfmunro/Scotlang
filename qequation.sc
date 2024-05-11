fn main ()
{
    a = 0;

    b = 0;

    c = 0;

    input(a);

    input(b);

    input(c);

    x = 0;

    if (a == 0)
    {
        x = solveLinearEquation (b, c);
    }
    
    if (a != 0)
    {
        x = solveQuadraticEquation (a, b, c);
    }

    ret;
}

fn solveLinearEquation (a, b, c)
{
    if (b == 0)
    {
        if (c != 0)
        {
            print (1);
        }

        if (c == 0)
        {
            print (1);

            print (0);
        }
    }

    if (b != 0)
    {
        x = - c / b;

        print(x);
    }

    ret 1;
}

fn solveQuadraticEquation (a, b, c)
{
    d = b ^ 2 - 4 * a * c;

    if (d < 0)
    {
        print (0);
    }
    if (d == 0)
    {
        x = - b / (2 * a);

        print(x);
    }  
    if (d > 0)
    {
        x = (- b + sqrt(d)) / (2 * a);

        y = (- b - sqrt(d)) / (2 * a);

        print (2);

        print (x);

        print (y);
    }

    ret 1;
}
