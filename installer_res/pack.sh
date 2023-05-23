
PKG_DATE=$(date '+[%Y-%m-%d]')
cp ../Changelog.md __base/Changelog.md
for subdir in 1966 1965 1964 1963 1953
do
    echo packing v$subdir into ../FMCB-$subdir.7z
    NEWDIR="FMCBinst-$subdir-$PKG_DATE"
    cp -r __base/ $NEWDIR/
    cp -r $subdir/INSTALL/ $NEWDIR/INSTALL/
    echo $SHA8>$NEWDIR/lang/commit.txt
    echo "title=FreeMcBoot v$subdir $PKG_DATE" >$NEWDIR/title.cfg
    echo "boot=FMCBInstaller.elf">>$NEWDIR/title.cfg
    cp FMCBInstaller.elf $NEWDIR/
    cp FMCBInstaller_EXFAT.elf $NEWDIR/
    7z a -t7z -r ../FMCB-$subdir.7z $NEWDIR/*
done
