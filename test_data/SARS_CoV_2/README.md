As input we use the file `metadata/nextstrain_ncov_open_global_all-time_metadata.tsv` 
as out start.

We filter out the lines that do not have a genbank accession numbers
and also randomly select at most 10 rows from each NextStrain strain to end up 
with `metadata/output_data.tsv`

