# Junctions

Compare ED strings

Supports the DNA alphabet: `{A, T, C, G, N, }`.

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
cat x.dot | ./scripts/dot2gfa > x.gfa
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

#### Using make
creates a dynamically linked binary
```
make
```

in case of need of statically linked binary
```
make static
```

for junctions to internally convert MSA to EDS only supported on newer processors
```
make WITH_MSA=true
```
or
```
make static WITH_MSA=true
```

## Usage
Run the following for the help text
```
$ ./bin/junctions
-----------------
```
