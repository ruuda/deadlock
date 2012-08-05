Compiling Deadlock
==================

Deadlock uses the [CMake](http://cmake.org/) build system
to handle configuration of the build system.
This makes it possible to build Deadlock on different platforms,
and for different architectures.


Dependencies
============

First of all, you must make sure that you have the required
development libraries of the dependencies on your system.

Boost
-----
[Boost](http://www.boost.org) is an extension to the Standard Template Library.
On Unix-like platforms, Boost will be present in the package repositories.
On Windows, you need to download the source from [boost.org](http://www.boost.org/).
You can then either compile Boost yourself, or get prebuilt binaries.
[Teeks99](http://boost.teeks99.com/) has excellent binaries for Visual Studio.

LibTomCrypt
-----------
Deadlock relies on [LibTomCrypt](http://libtom.org/?page=features&whatfile=crypt)
for encryption. I recommend building the library yourself,
which is easy to do and does not take long (unlike Boost).
You can clone the [git repository](https://github.com/libtom/libtomcrypt)
or download an archive from the website.

A Visual Studio project is included with the library.
To build a 64-bit binary, simply add a new target to the project.

XZ Utils
--------
Vault contents are compressed before they are encrypted,
using LZMA2 encryption. Deadlock uses [XZ Utils](http://tukaani.org/xz/) (liblzma)
for this. On Windows, you can either build the libraries yourself, or use
one of the pre-built versions. On Unix-like systems, you probably have liblzma
installed already, or you can install it via your package manager.

Generating a configuration with CMake
=====================================

Once you have the dependencies, you can configure a project
or makefiles with CMake. If you run CMake for the first time,
you will need to tell it where it can find the dependencies.
If it fails to find Boost, you must manually set
`Boost_INCLUDE_DIR` and `Boost_LIBRARY_DIRS` to the correct paths.
On a Unix-like platform, also make sure that the libraries
(Chrono, Program Options and System) use the
dynamically linked versions of the library.


Compiling
=========
Compiling should now be trivial.
The `tests` executable will verify that the
Deadlock core library works correctly.

