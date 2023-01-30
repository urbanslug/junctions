# Human Genome Experiments

The [GRCh38 dataset][1] chromosome 6 download link https://ftp.ensembl.org/pub/release-108/fasta/homo_sapiens/dna/Homo_sapiens.GRCh38.dna.chromosome.6.fa.gz

The [variation data][2]:
 - clinically associated https://ftp.ensembl.org/pub/release-108/variation/vcf/homo_sapiens/homo_sapiens_clinically_associated.vcf.gz
 - generic variation data https://ftp.ensembl.org/pub/release-108/variation/vcf/homo_sapiens/homo_sapiens-chr6.vcf.gz

## LPA

Why these coordinates? Consult the [Ensembl LPA page](http://www.ensembl.org/Homo_sapiens/Gene/Summary?g=ENSG00000198670;r=6:160531482-160664275)

bedfile
```
6	160531482	160664275
```

bedtools command to isolate the LPA region
```
bedtools getfasta -fi Homo_sapiens.GRCh38.dna.chromosome.6.fa -bed lpa.bed -fo chr6.lpa.fa
```

subset the generic vcf
```
sed -n '38059999,38060999p' homo_sapiens-chr6.vcf > homo_sapiens-chr6.lpa.subset.vcf
```

Build the ed string with [aedso](https://github.com/urbanslug/aedso)
```
aedso -s 160531482 -e 160664275 chr6.lpa.fa homo_sapiens-chr6.lpa.subset.vcf > x.eds
aedso -s 160531482 -e 160664275 chr6.lpa.fa homo_sapiens_clinically_associated_chr6.vcf > y.eds
```

and comapre using junctions 
```
junctions -v=1 -a=2 -w x.eds -q y.eds
```

## HLA 1 B

[Ensembl HLA-B page](http://www.ensembl.org/Homo_sapiens/Gene/Summary?g=ENSG00000234745;r=6:31353872-31367067;transcript=ENST00000696560.1)

bedfile
```
6	31353872	31367067
```

bedtools command to isolate the HLA region
```
bedtools getfasta -fi Homo_sapiens.GRCh38.dna.chromosome.6.fa -bed hla.bed -fo chr6.hla.fa
```

Subset the generic VCF
```
sed -n '7465083,7472483p' homo_sapiens-chr6.vcf > homo_sapiens-chr6.hla.subset.vcf
```

```
aedso -s 31353872 -e 31367067 chr6.hla.fa homo_sapiens-chr6.hla.subset.vcf > j.eds
aedso -s 31353872 -e 31367067 chr6.hla.fa homo_sapiens_clinically_associated.vcf > k.eds
```

```
junctions -v=1 -a=2 -w j.eds -q k.eds
```

[1]: https://ftp.ensembl.org/pub/release-108/fasta/homo_sapiens/dna/
[2]: https://ftp.ensembl.org/pub/release-108/variation/vcf/homo_sapiens/
