# Running 

Scripts to convert an ED String in 

Expects input in 
row-column multiple sequence alignment in FASTA format


3 algorithms:
 - trivial 
 - greedy 
 - double-greedy
 
Run with:

```sh
python3 seq_to_ed.py msa_file_name eds_file_name algo_name
```

Example:

```sh
python3 seq_to_ed.py foo.msa foo.eds trivial
```

#### Attribution
Code from Michelle Sweering
