# Pipeline

It's advisable to add this scripts dir to `PATH` and work from the data dir

```
export PATH="$PATH:<path_to_junctions>/scripts/pipeline"
```

Test that it worked by seeing the help text after running `pw`.

## Overview

| Script | Tasks                                                                                 |
| ------ | --------------------------------------------------------------------------------------|
| seq_dl | fetch sequences from genbank                                                          |
| do_msa | takes processed TSV breaks it into clades and uses abPOA to generate and MSAs (files) |
| pw     | runs pairwise junctions jobs                                                          |
| gen_tsv| uses the data from `pw` (above) to generate a TSV table of the output from junctions            |

### Fetch data

use `seq_dl` to download the necessary data from a file such as `output_data.tsv`
example: `seq_dl output.tsv all_sequences.fasta`

### Generate Multiple Sequence Alignments

use `do_msa` to generate multiple sequence alignments.
`do_msa` takes an optional output dir
`do_msa output_data.tsv all_sequences.fasta <output_dir>`


### Run pairwise comparison jobs (in parallel)

Run a series of pairwise matching statistics average with:
`./pw` for help run `./pw --help`

### generate TSVs
use the script `gen_tsv` to convert the contents of pairwise matching statistics `pw` from a text file to a TSV file.
Example:
assuming the output from `pw` is `res3.txt` and the aim is to output `res3.tsv`

`./gen_tsv <in_file> <out_file> <with_time>`
`./gen_tsv res3.txt res3.tsv t`

