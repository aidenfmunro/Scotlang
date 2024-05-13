dae main fit like?
ya
    a gie 0 !

    b gie 0 !

    c gie 0 !

    speir fit a like? !

    speir fit b like? !

    speir fit c like? !

    x gie 0 !

    fa fit a div 0 like?
    ya
        x gie lineareq fit b, c like? !
    gan?
    
    fa fit a wisnae 0 like?
    ya
        x gie quadreq fit a, b, c like? !
    gan?

    wheesht !
gan?

dae lineareq fit b, c like?
ya
    fa fit b div 0 like?
    ya
        fa fit c wisnae 0 like?
        ya
            spikin fit 1 like? !
        gan?

        fa fit c div 0 like?
        ya
            spikin fit 1 like? !

            spikin fit 0 like? !
        gan?
    gan?

    fa fit b wisnae 0 like?
    ya
        x gie fit c - c - c like? / b !

        spikin fit x like? !
    gan?

    wheesht 1 !
gan?

dae quadreq fit a, b, c like?
ya
    d gie b * b - 4 * a * c !

    spikin fit d like? !

    fa fit d sma 0 like?
    ya
        spikin fit 0 like? !
    gan?
    fa fit d div 0 like?
    ya
        x gie fit b - b - b like? / fit 2 * a like? !

        spikin fit x like? !
    gan?  
    fa fit d mair 0 like?
    ya
        bm gie b - b - b !
        ds gie sqrt fit d like? !

        x gie fit bm + ds like? / fit 2 * a like? !

        y gie fit bm - ds like? / fit 2 * a like? !

        spikin fit x like? !

        spikin fit y like? !
    gan?

    wheesht 1 !
gan?
