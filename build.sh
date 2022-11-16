echo "parameter 1 $1"
echo "RESTORE_BASE_PACKAGES: $RESTORE_BASE_PACKAGES"
echo "cc qcmdln.c"
cc qcmdln.c
echo "mv a.out qcmdln"
mv a.out qcmdln
echo "cp -p qcmdln $HOME/bin"
cp -p qcmdln $HOME/bin
echo "build.sh is done"
echo "what happens next?"
echo "checking the workflow?"
