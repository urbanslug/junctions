# Junctions

This is a software suite for pangenome comparison via elastic-degenerate (ED) strings.

An ED string is a sequence of sets of strings. We currently support only the DNA alphabet `{A, T, C, G}`, the letter `N` for the indeterminate base, and the empty string `Ɛ`. For example, the ED string x below has 3 sets. The first set has two strings (`AGT` and `A`); the second set has three strings (`A`, `T`, and the empty string `Ɛ`), and the third set has one string (`ACGTN`).

```
$ cat x.eds
{AGT,A}{A,T,}{ACGTN}
```

## Download the source code

Using `git`
```sh
git clone https://github.com/urbanslug/junctions
cd junctions
```

or using `curl` and `zip`:
```
curl -LO https://github.com/urbanslug/junctions/archive/refs/heads/master.zip
unzip master.zip
cd junctions
```

## Compile

Compilation is done with `make` and can be done in different ways.

To create a dynamically linked binary (advisable)
```
make
```

in case of need of statically linked binary
```
make static
```

When compiled with MSA support junctions is able to internally convert MSA 
files in RC-MSA format to ED strings however this is only supported on newer 
x86 processors.

To compile a dynamically linked binary
```
make WITH_MSA=true
```

or for a statically linked binary
```
make static WITH_MSA=true
```

## Usage and Documentation
Run the following for the help text

```
./bin/junctions
```

Further documentation can be found in the [wiki](https://github.com/urbanslug/junctions/wiki).


## Example 1: ED string intersection
Consider two ED strings x and y encoded in the corresponding files below:

```
$ cat x.eds 
{A,AC,TGCT}{CA,}
```

```
$ cat y.eds 
{,T}{GCA,AC}

```
We can determine whether x and y have a nonempty intersection by running the following

```
$ ./bin/junctions intersect -a 0 x.eds y.eds 
INFO intersection exists
```
Indeed, x and y share the string `AC`.
