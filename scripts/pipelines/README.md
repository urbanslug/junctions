# Snakefile for Graph and Intersection Pipelines
This Snakefile provides two pipelines, one for generating graphs and another for intersection comparisons. The configuration is set in the config.yaml file but can be passed in through the terminal. 


Follow the instructions below to use and customize the pipelines.


## Configuration
`in_dir`:  Path to the directory containing input files.
`out_dir`: Path to the output directory.
`mode`: Choose between `-d` for distance or `-s` for similarity.


## Usage
### Update Configuration

Edit the config.yaml file to set your preferred mode and input paths. 
Modes are `-d` for distance and `-s` for similarity

Run a pipeline from the CLI like so:

Graph pipeline
```
snakemake graph --configfile config.yaml
```


With values passed in the CLI
```
snakemake graph --config mode="-d" in_dir="/path/to/your/files" out_dir="dist"
```


An example run of the **intersect** pipeline is below
 
```
snakemake intersect --config out_dir="inter"
```


To run both intersect and graph pipelines run
 
```
snakemake --configfile config.yaml
```

in this case it will use the distance mode and if later ran for similarity mode
it may overwrite files in output directory if not updated

The output will be concatenated into a file `all.txt` in `out_dir`

## Other scripts

Concatenate benchmark results into a single file
```
python bench
```

Convert the concatenated TSV file (benchmark results) `./out/accumulated_results.txt` into an org table
```
cat ./out/accumulated_results.txt | python tsv_to_org > tbl.org
```

Convert pairwise distances in TSV from the pipeline to an org table

```
cat ./out/all.txt | python ogre.py
```


## Tips

run with more cores

```
snakemake --cores 8 ...
```
