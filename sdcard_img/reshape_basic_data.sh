#!/bin/sh
sed -e 's/:/=/' ./train/basic.yaml > reshape_data.txt
#sed -e 's/$/&;/' tmp > reshape_data.txt
sed -e 's/$/&;/' -i reshape_data.txt
sed -e '11s/^/rng->/' -i reshape_data.txt
sed -e '12,14s/^/params.term_crit./' -i reshape_data.txt
sed -e '15,$s/^/params./' -i reshape_data.txt
