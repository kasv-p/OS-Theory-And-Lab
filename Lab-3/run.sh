echo "copying files"
cp schedule.c /usr/src/minix/servers/sched/schedule.c
echo "going to src directory and building the updated code"
cd /usr/src
make build MKUPDATE=yes >log.txt 2>log.txt
echo "build completed successfully"
exit 0