clear

echo "=============================================== Updating the code ==============================================="

git clean -fd
git pull --rebase


echo "\n=============== Removing previously inserted module, zero device and device-mapper proxy device ==============="

dmsetup remove dmp1
dmsetup remove zero1
rmmod -f dmp.ko


echo "\n============================================== Building the module ============================================="

make
insmod dmp.ko


echo "\n=============================================== Creating devices =============================================="

size=1953125 # 1GB

echo "Creating a zero device..."
dmsetup create zero1 --table "0 $size zero"
ls -al /dev/mapper/*

echo "\nCreating a device-mapper proxy device..."
dmsetup create dmp1 --table "0 $size dmp /dev/mapper/zero1"
ls -al /dev/mapper/*


echo "\n============================================== Testing the module ============================================="

# Clear the statistics file
echo > /sys/module/dmp/stat/volumes

# Run some read and write operations
dd if=/dev/random of=/dev/mapper/dmp1 bs=4k count=1
dd of=/dev/null if=/dev/mapper/dmp1 bs=4k count=1


echo "\n=============================================== Module statistics ============================================="

# Display the statistics
cat /sys/module/dmp/stat/volumes
