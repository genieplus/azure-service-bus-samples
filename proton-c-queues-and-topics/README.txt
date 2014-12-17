Linux
=====

These samples have been tested with Proton-C versions 0.4 through 0.8.

1) For version 0.4, use Makefile.0.4. Other versions use Makefile.
2) At the top of the appropriate makefile:
  a) Change PROTONVER to the version of Proton-C that you're using.
  b) Change PROTONROOT to point to the location of your build of Proton-C.
  c) Change CFLAGS to define SERVICEBUS_DOMAIN.
3) Run "make all" or "make -f Makefile.0.4 all" as appropriate.


Windows
=======

These samples have been tested with Proton-C version 0.6 built with OpenSSL.

1) At the top of Makefile.win32:
  a) Change PROTONVER to the version of Proton-C that you're using.
  b) Change PROTONROOT to point to the location of your build of Proton-C.
  c) Change PROTONCONFIG to be the configuration you used to build Proton-C.
  d) Change CFLAGS to define SERVICEBUS_DOMAIN.
2) In a Visual Studio x86 command prompt, run "nmake /f Makefile.win32 all".
