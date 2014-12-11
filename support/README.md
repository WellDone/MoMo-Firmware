##Support Files
These files are support for external tools that may require modification in order to work with MoMo.  Currently the only such tool is gpsim, which requires a patch in order to correctly simulate Enhanced Midrange PIC processors.  In order to apply the patch you need to download gpsim from svn, apply the patch.

This document assumes that you are checking out gpsim into the parent directory containing MoMo-Firmware.  If this is not the case you will need to specify
the path to gpsim_16lf1847_support.patch based on your system.

```shell
> mkdir gpsim
> cd gpsim
> svn checkout svn://svn.code.sf.net/p/gpsim/code/trunk -r 2281 .
<Lots of text>
> patch -p0 -i ../MoMo-Firmware/support/gpsim_16lf1847_support.patch
patching file src/14bit-hexdecode.cc
patching file src/14bit-instructions.h
patching file src/Makefile.am
patching file src/makefile.mingw
patching file src/p16f1847.cc
patching file src/p16f1847.h
patching file src/p1xf1xxx.cc
patching file src/p1xf1xxx.h
patching file src/pic-instructions.cc
patching file src/pic-processor.cc
patching file src/pic-processor.h
```

Now you need to follow the [instructions](http://gpsim.sourceforge.net/gpsim_svn.html) to build and install gpsim.  For reference, the current required commands are:

```shell
> libtoolize --force
> aclocal
> autoheader
> automake --force-missing --add-missing
> autoconf
> ./configure
> make
> sudo make install
```

Make sure you install gpsim into your $PATH correctly and you should be able to build pic12 unit tests.  As a test, navigate to the pic12_executive directory
and make sure you can build the tests:
```shell
> cd MoMo-Firmware/momo_modules/pic12_executive
> momo build -c
> momo build test
```

You need to have the pro mode compiler currently to build the pic12_executive. 