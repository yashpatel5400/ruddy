fn test
    x = 3 
    y = 5 * 1 + 2

    z = "hello world"
    aasdf = "test "

    print(x + y - 3)
    print("testing hello world")
    print(aasdf + z)
endfn

fn main
    x = 3
    y = 4

    if x > y
        test
    else
        if x < y
            test
        else
            print("NOPE2")
        endif

        print("NOPE")
    endif
endfn 