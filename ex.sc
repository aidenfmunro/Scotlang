fn main ()
{
    if (a == 0)
    {
        solveLinearEquation (b, c);
    }
    
    if (a != 0)
    {
        solveQuadraticEquation (a, b, c);
    }
}

fn solveLinearEquation (a, b, c)
{
    if (b == 0)
    {
        if (c != 0)
        {
            print (-1);
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
        x1 = (- b + sqrt(d)) / (2 * a);

        x2 = (- b - sqrt(d)) / (2 * a);

        print (2);

        print (x1);

        print (x2);
    }
}




