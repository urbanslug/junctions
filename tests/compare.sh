D=$1
S=$2
L=$3
W=$4
ALGO=$5
OUTDIR=$6
RUNS=$7

if [ -z "$6" ]
  then
    # echo ""
    OUT_DIR=/tmp/junctions
  else
	  OUT_DIR=$6
fi

if [ -z "$5" ]
  then
    # echo ""
    ALGO=2
fi

if [ -z "$7" ]
  then
    # echo ""
    RUNS=3
fi



#d=d$d
#s=s$s
# l=l$l
# w=w${W}k
OUT=${OUT_DIR}/out.d$D.s$S.l$L.w${W}k.time

w=x1.d$D.s$S.l$L.w${W}k.eds
q=x2.d$D.s$S.l$L.w${W}k.eds
now=$(date "+%Y-%m-%dT%H:%M:%S")

echo "w: ${w}"
echo "q: ${q}"
echo "runs ${RUNS}"

for (( c=1; c<=$RUNS; c++ ))
do
	echo $now >> $OUT
	# $( /usr/bin/time -f"%S\t%M" junctions -v=1 -a=$ALGO -w $w -q $q 2>&1 | tail -n 4 >> $OUT )
	$( /usr/bin/time -f"%S\t%M" junctions -v=1 -a=$ALGO -w $w -q $q 2>&1 | cat >> $OUT )
done
