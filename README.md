# Junctions

Compare ED strings

Takes at least one of two args

 - `-i` checks whether the intersection exists
 - `-g` constructs the intersection graph

## ED String Comparison Tasks
These only work with `-g`.

### Longest or shortest witness
If an intersection exists get the length of the longest string from the start to
end of the intersection graph.<br>
Uses the `-w <witness_type>` where witness type is longest/long/1 or shortest/short/0.

### Size of the multiset
Get the number of unique strings in the language of `T₁` and `T₂` using `-m`.

### Matching statistics
Find the length of the longest prefix of a string starting at some letter which
we specify with `-n` in either the string `1` or `2` which we pass with `-T` and
is a substring in the other elastic degenerate string.

## Visualizing the intersection graph

It's possible to output the intersection graph in dot format using the `-d`
or `--print-dot` option.

For example:

```
junctions -g -d data/x.eds data/y.eds > x.dot
```

This can later be visualized using  `dot -Tsvg -o x.svg x.dot` or
`dot -Tpng -o x.png x.dot`


For larger graphs you may want to visualize them in
[bandage](https://github.com/rrwick/Bandage/) so there's a quick
`dot` to `gfa` converter which lets one open the `gfa` graph in
[bandage](https://github.com/rrwick/Bandage/)

```
cat x.dot | python scripts/dot2gfa.py > x.gfa
```

## Compile and Run

### Download the source code
using git
```sh
git clone https://github.com/urbanslug/junctions
cd junctions
```

using curl and zip
```
curl -LO https://github.com/urbanslug/junctions/archive/refs/heads/master.zip
unzip master.zip
cd junctions
```

### Compile
```
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build -- -j 3 
```

### Run

```
./bin/junctions
```

## Usage

```
$ ./bin/junctions
-----------------
Compute the intersection of Elastic Degenerate Strings
Provide at at least -i to check whether and intersection exists,
or -g to compute the intersection graph
Example usage:
check whether the intersection exists
$ ./bin/junctions -i -a=2 data/x.eds data/y.eds
compute the intersection graph
$ ./bin/junctions -g -d data/x.eds data/y.eds

Available options
-----------------
-h, --help
    Print this help page

-e, --info
    print info about the given ed string(s) and exit

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
    T₁ = 1
    T₂ = 2
    [default: 1]

--match-stats-idx <value>, -n <value>
    matching stats letter index

-v <value>, --verbosity <value>
    amount of debug information [default : 0]
```

## Running tests and Debugging
Assuming a user is in the `junctions/` dir create a debug build
```sh
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build -- -j 3
cd build
```

Run [googletest](https://github.com/google/googletest) tests
```
ctest
```

both of these assume you're within build/
```sh
gdb junctions # to debug the binary without tests
gdb junctions_test # to debug the tests binary
```
