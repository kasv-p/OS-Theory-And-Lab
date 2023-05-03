echo "copying files";
cp system.c /usr/src/minix/kernel/;
echo "going to src directory and building the updated code";
cd /usr/src/;
make build MKUPDATE=yes >log.txt 2>log.txt;
echo "build completed successfully";
exit 0;
