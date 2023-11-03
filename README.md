# Junctions

Compare ED strings

Supports the DNA alphabet `{A, T, C, G}` as well as `{N, Ɛ}` for the indeterminate base and empty string respectively.

## Download the source code
using `git`
```sh
git clone https://github.com/urbanslug/junctions
cd junctions
```

or using `curl` and `zip`
```
curl -LO https://github.com/urbanslug/junctions/archive/refs/heads/master.zip
unzip master.zip
cd junctions
```

## Compile

Compilation is done with `make` and can be done in different ways.

To creates a dynamically linked binary (advisable)
```
make
```

in case of need of statically linked binary
```
make static
```


When compiled with MSA support junctions is able to internally convert MSA 
files in RC-MSA format to ED-Strings however this is only supported on newer 
intel processors.

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
