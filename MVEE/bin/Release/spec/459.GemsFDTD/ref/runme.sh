BENCHPATH=$SPECPATH/459.GemsFDTD
BINPATH=$BENCHPATH/build/$SPECPROFILE
DATAPATH=$BENCHPATH/data/ref/input
ALLDATAPATH=$BENCHPATH/data/all/input

cd $MVEEROOT/MVEE/bin/Release/spec/mvee_run/459.GemsFDTD
$BINPATH/GemsFDTD > Gems.out
