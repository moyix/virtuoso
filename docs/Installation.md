# Virtuoso Installation Guide #

**Requirements**:

  * 64-bit Linux system (Debian/Ubuntu preferred)
  * Python 2.6 or later

## Step 1: Obtain the Virtuoso source ##

You can get the source code to Virtuoso at:

```
http://code.google.com/p/virtuoso/downloads/list
```

At the time of this writing, the latest version is 1.0.

## Step 2: Install dependencies ##

Virtuoso has 3 main components:

  * iFerret, a modified version of QEMU that can log every instruction executed by the operating sytem
  * dynslicer, a set of python scripts that takes the traces produced by iFerret and converts them into a script usable by the Virtuoso runtime environment
  * microdo, a Volatility plugin that provides a runtime environment for the code translated by dynslicer

The only one of these that has any serious dependencies is iFerret, which requires everything QEMU 0.9.1 requires plus a few more things. The others just need a recent version of Python (2.6 or newer).

2a. Install the dependencies for QEMU

If you're on Debian or Ubuntu, you're in luck: you can just run

```
apt-get build-dep qemu
```

to install almost everything needed to build QEMU.

Otherwise, you'll have to look at the QEMU documentation to see what is needed on your distribution.

2b. Build and install libdasm

Grab libdasm from

```
http://code.google.com/p/libdasm/
```

Either the beta or the 1.5 release should work fine.

To build, unpack the source and then, in the libdasm directory:

```
make
sudo make install
cd pydasm
sudo python setup.py install
```

2c. Install gcc-3.4

QEMU 0.9.1 requires gcc 3.4, which is getting rather old by now, and it can be tricky to track down working packages for newer distributions. Stephen McCamant has a good discussion of the problem, and its fix, here:

```
http://groups.google.com/group/bitblaze-users/browse_thread/thread/cb900a87c308eebb
```

For Virtuoso on Ubuntu, you'll want to run:

```
url=http://us.archive.ubuntu.com/ubuntu/pool/universe/g/gcc-3.4 
version=3.4.6-6ubuntu5 
arch=amd64
wget $url/gcc-3.4-base_${version}_$arch.deb 
wget $url/cpp-3.4_${version}_$arch.deb 
wget $url/gcc-3.4_${version}_$arch.deb 
sudo dpkg -i gcc-3.4-base_${version}_$arch.deb \ 
             cpp-3.4_${version}_$arch.deb \ 
             gcc-3.4_${version}_$arch.deb
```

2d. Fix libgcc`_`s

The location of the libgcc`_`s libary has changed in recent Ubuntu distributions, so you'll need to create a symlink to the correct location:

```
sudo ln -sf /lib/x86_64-linux-gnu/libgcc_s.so.1 \
            /usr/lib/gcc/x86_64-linux-gnu/3.4.6/libgcc_s.so
```

## Step 3: Compile iFerret ##

In the Virtuoso source directory, go into the iferret-logging-new directory and issue:

```
./cleanup.sh
make install
make -f Makefile-iferretso
```

This will build and install iFerret. Note that by default, cleanup.sh installs iFerret into iferret-logging-new/install/bin/qemu, so you do not need root privileges to run make install. If you want to change where iFerret is installed, you can edit cleanup.sh.

## Conclusion ##

At this point, everything should be set up so that you can run iFerret and generate traces, translate them into Volatility plugins, and then run the generated plugins against memory images. Check out the [Walkthrough](Walkthrough.md) document for an overview of what to do from here.