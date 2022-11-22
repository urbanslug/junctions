# Junctions
Do two elastic degenerate strings have a non-empty intersection?


## Compile and Run
Compile
```
git clone https://github.com/urbanslug/junctions.git && cd junctions
make
```

Run
```
$ ./build/junctions -h
-----------------
Compute the intersection of Elastic Degenerate Strings
Example usage:
$ ./build/junctions -a=2 -w data/x.eds -q data/y.eds

Available options
-----------------
-h, --help
    Print this help page

-w <value>
    a file containing an msa (in RC-MSA format) or ED-String in .eds format

-q <value>
    a file containing an msa (in RC-MSA format) or ED-String in .eds format

-a, --algorithm
    algorithm to use:
    improved = 0
    naive = 1
    both = 2
    [default: 0]

-v <value>, --verbosity <value>
    amount of debug information [default : 0]
```
