<h1 align="center">Device Mapper Proxy (dmp) module with statistics</h1>

Device Mapper Proxy (dmp) is a kernel module for the Linux operating system that creates virtual block devices on top of existing devices using the [device mapper framework](https://en.wikipedia.org/wiki/Device_mapper). The module also monitors and provides statistics on the performed operations, which are accessible through the [`sysfs`](https://en.wikipedia.org/wiki/Sysfs) interface.

## Remarks

I used `Ubuntu 22.04.3 LTS` with the `5.15.0-92-generic` kernel for building and testing the module

## Installation

Clone the repository:

```bash
git clone https://github.com/npanuhin/edu-dmp-stats-module
```

## Building and Testing

To build and intially test the module, run `run.sh`. It will give a nice summary of the operations performed and will run a simple test to check the module's functionality.

```bash
sh ./run.sh
```

<details>
<summary>Click to see the output</summary>

```bash
=============================================== Updating the code ===============================================
Removing .Module.symvers.cmd
Removing .dmp.ko.cmd
Removing .dmp.mod.cmd
Removing .dmp.mod.o.cmd
Removing .dmp.o.cmd
Removing .modules.order.cmd
Removing Module.symvers
Removing dmp.ko
Removing dmp.mod
Removing dmp.mod.c
Removing dmp.mod.o
Removing dmp.o
Removing modules.order
Already up to date.

=============== Removing previously inserted module, zero device and device-mapper proxy device ===============

============================================== Building the module =============================================
make -C /lib/modules/5.15.0-92-generic/build M=/home/npanuhin/tools/edu-dm-proxy-stats-module modules
make[1]: Entering directory '/usr/src/linux-headers-5.15.0-92-generic'
  CC [M]  /home/npanuhin/tools/edu-dm-proxy-stats-module/dmp.o
  MODPOST /home/npanuhin/tools/edu-dm-proxy-stats-module/Module.symvers
  CC [M]  /home/npanuhin/tools/edu-dm-proxy-stats-module/dmp.mod.o
  LD [M]  /home/npanuhin/tools/edu-dm-proxy-stats-module/dmp.ko
  BTF [M] /home/npanuhin/tools/edu-dm-proxy-stats-module/dmp.ko
Skipping BTF generation for /home/npanuhin/tools/edu-dm-proxy-stats-module/dmp.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.15.0-92-generic'

=============================================== Creating devices ==============================================
crw------- 1 root root 10, 236 фев  9 00:10 /dev/mapper/control
lrwxrwxrwx 1 root root       7 фев  9 00:17 /dev/mapper/zero1 -> ../dm-0
crw------- 1 root root 10, 236 фев  9 00:10 /dev/mapper/control
lrwxrwxrwx 1 root root       7 фев  9 00:17 /dev/mapper/dmp1 -> ../dm-1
lrwxrwxrwx 1 root root       7 фев  9 00:17 /dev/mapper/zero1 -> ../dm-0

============================================== Testing the module =============================================
1+0 records in
1+0 records out
4096 bytes (4,1 kB, 4,0 KiB) copied, 0,00151003 s, 2,7 MB/s
1+0 records in
1+0 records out
4096 bytes (4,1 kB, 4,0 KiB) copied, 0,000888471 s, 4,6 MB/s

=============================================== Module statistics =============================================
read:
  reqs: 2286
  avg size: 1004
write:
  reqs: 8
  avg size: 512
total:
  reqs: 2294
  avg size: 1003
```
</details>

## Further Usage

- To reset the statistics, run:

	```bash
	echo > /sys/module/dmp/stat/volumes
	```

- To view the statistics, run:

	```bash
	cat /sys/module/dmp/stat/volumes
	```

	<details>
	<summary>Click to see the sample output</summary>

	```bash
	read:
	  reqs: 83
	  avg size: 1875
	write:
	  reqs: 0
	  avg size: 0
	total:
	  reqs: 83
	  avg size: 1875
	```
	</details>
