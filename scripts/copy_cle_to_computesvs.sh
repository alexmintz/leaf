#!/bin/bash
#HOSTS="rh5x64-u25 rh5x64-u26 rh5x64-u27 rh5x64-u28 rh5x64-u29 rh5x64-u30 rh5x64-u31 rh5x64-u32 rh5x64-u33 rh5x64-u37 rh5x64-u39"
SCRIPT="cd /CQG_MD/depth_backtest; tar -xzf cle.tgz"
for HOSTNAME in ${HOSTS} ; do
	scp /teradata/CQG_MD/cle.tgz ${LOGNAME}@${HOSTNAME}:/CQG_MD/depth_backtest
	ssh -l ${LOGNAME} ${HOSTNAME} "${SCRIPT}"
done
