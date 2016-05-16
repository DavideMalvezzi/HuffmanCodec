cd original
md5sum *.* >> md5.txt
cd ..
mv original/md5.txt decompressed/md5.txt
cd decompressed
md5sum -c md5.txt
rm -f md5.txt
cd ..
