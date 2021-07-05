# ruddy
Ruddy is an experimental language, mostly for educational purposes. The syntax is fairly clunky (especially for function dispatch), but this is an example:

```
fn test
    x = 3 // this is a comment
    y = 5 * 1 + 2

    z = "hello world"
    a = "test "

    print(x + y - 3)
    print("testing hello world")
    print(a + z)
endfn

fn main
    test
    print("exit main")
endfn

```