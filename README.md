# Junctions

Compare ED strings

Takes at least one of two args

 - `-i` checks whether the intersection exists
 - `-g` constructs the intersection graph

## ED String Comparison Tasks
These only work with `-g`.

### Longest or shortest witness
If an intersection exists get the length of the longest string from the start to
end of the intersection graph. Uses the `-w <witness_type>` where witness type
is longest/long/1 or shortest/short/0.

### Size of the multiset
Get the number of unique strings in the language of `T_1` and `T_2` using ` -m`.

### Matching statistics
Find the length of the longest prefix of a string starting at some letter which
we specify with `-n` in either the string `1` or `2` which we pass with `-T` and
is a substring in the other elastic degenerate string.

## Visualizing the intersection graph

It's possible to output the intersection graph in dot format using the `-d`
or `--print-dot` option.

For example:

`junctions -g -d data/x.eds data/y.eds > x.dot`

This can later be visualized using  `dot -Tsvg -o x.svg x.dot` or
`dot -Tpng -o x.png x.dot`

## Compile and Run
Compile
```sh
git clone https://github.com/urbanslug/junctions && cd junctions
make
```

Run
```sh
$ ./bin/junctions
-----------------
Compute the intersection of Elastic Degenerate Strings
Provide at at least -i to check whether and intersection exists,
or -g to compute the intersection graph
Example usage:
check whether the intersection exists
$ ./build/junctions -i -a=2 data/x.eds data/y.eds
compute the intersection graph
$ ./build/junctions -g -d data/x.eds data/y.eds

Available options
-----------------
-h, --help
    Print this help page

-i, --intersect
    check whether the intersection is non-empty

-a <value>, --algorithm <value>
    (used with -i) algorithm to use:
    improved = 0
    naive = 1
    both = 2
    [default: 0]

-g, --graph
    compute the intersection graph

-d, --print-dot
    output the intersection graph in dot format [default: false]

-m, --multiset
    compute the size of the multiset

-w <value>, --witness <value>
    compute the shortest or longest witness
    shortest = short/shortest/0
    longest = short/longest/1

-T <value>, --match-stats-str <value>
    matching stats ed string
    T_1 = 1
    T_2 = 2
    [default: 1]

--match-stats-idx <value>, -n <value>
    matching stats letter index

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
