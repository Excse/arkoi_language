# Functions

The declaration of a function is always ``fun <name>(<parameter list>) <type>`` and can be divided between short and 
long functions as described in the examples.

### Long Function
```c
fun test() @i32 {
    return 1;
}
```

### Short Function
```c
fun test() @i32 => 1;
```

The main difference is, that a short function is useful for getters, setters or other very trivial functions and thus is
a "one liner". Generally there is no ``return`` inside a short function as the very first expression will be the result 
of it. In summary those two "types" are just syntax sugar for the same thing. 