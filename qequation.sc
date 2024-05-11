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
        x = lineareq (b, c);
    }
    
    if (a != 0)
    {
        x = quadreq (a, b, c);
    }

    ret;
}

fn lineareq (b, c)
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

fn quadreq (a, b, c)
{
    d = b * b - 4 * a * c;

    print(d);

    if (d < 0)
    {
        print (0);
    }
    if (d == 0)
    {
        x = (b - b - b) / (2 * a);

        print(x);
    }  
    if (d > 0)
    {
        bm = b - b - b;
        ds = sqrt(d);

        x = (bm + ds) / (2 * a);

        y = (bm - ds) / (2 * a);

        print (x);

        print (y);
    }

    ret 1;
}
