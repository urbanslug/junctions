# Junctions
Do two elastic degenerate strings have a non-empty intersection?


## Compile and Run
Compile
```sh
git clone https://github.com/urbanslug/junctions && cd junctions
make
```

Run
```sh
$ ./bin/junctions -h
-----------------
Compute the intersection of Elastic Degenerate Strings
Example usage:
$ ./bin/junctions -a=2 -w data/x.eds -q data/y.eds

Available options
-----------------
-h, --help
    Print this help page

-w <value>
    a file containing an ED-String in .eds format

-q <value>
    a file containing an ED-String in .eds format

-a, --algorithm
    algorithm to use:
    improved = 0
    naive = 1
    both = 2
    [default: 0]

-v <value>, --verbosity <value>
    amount of debug information [default : 0]
```

## Test and Debug
```sh
make test
./bin/test
```

```sh
make debug
gdb bin/test_debug
```
