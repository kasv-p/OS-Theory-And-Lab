echo "copying files"
cp -f open.c /usr/src/minix/servers/vfs/open.c
cp -f link.c /usr/src/minix/servers/vfs/link.c
cp -f read.c /usr/src/minix/servers/vfs/read.c
echo "going to src directory and building the updated code"
cd /usr/src
make build MKUPDATE=yes
echo "build completed successfully"
exit 0
