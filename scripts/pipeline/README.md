# Pipeline Scripts

Run a series of pairwise matching statistics average with:
`./pw` for help run `./pw --help`

use the script `gen_tsv` to convert the contents of pairwise matching statistics `pw` from a text file to a TSV file.
Example:
assuming the output from `pw` is `res3.txt` and the aim is to output `res3.tsv`

`./gen_tsv <in_file> <out_file> <with_time>`
`./gen_tsv res3.txt res3.tsv t`

