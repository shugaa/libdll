libdll is a doubly linked list implementation in C. Here's some instructions on
how to build and install it. I have tested this on Linux but it should work on
pretty much any Unix like operating system. Porting it to something else won't
be too hard either.

1. Create Makefiles (cmake needed)
    
    If you want to install into a non-standard location you can use
    CMAKE_INSTALL_PREFIX to change the destination:

        -DCMAKE_INSTALL_PREFIX:PATH=/opt/libdll

    You can use this cmake option to build with or without tests. You will need
    to have cunit (http://cunit.sourceforge.net/) installed in order to use
    this:

        -DDLL_WITH_TESTS:BOOL=TRUE

    I would suggest building out of tree, so you might want to make yourself a
    build directory somewhere:

        mkdir build
        cd build

    Finally run cmake with any of the above options. I prefixed the cmake
    command with the CFLAGS that I wanted to use, you will probably want to use
    your own ones. If in doubt just omit any CFLAGS:

        CFLAGS="-march=nocona -O2 -pipe -Wall -Werror" cmake [OPTS] /path/to/libdll/src/

2. Build
    
    Running make in your build direcotory will build the shared libdll library
    and optionally a 'dlltest' binary which runs some unit tests on the library:

        make

3. Install

    There is no need to install if for instance you just want to run dlltest.
    cmake will set everything up accordingly. Just go ahead and run test/dlltest.
    I would suggest to do so before installing anyway. 

        make install

4. Update your dynamic linker

    You might need to change the ld configuration to include any custom install
    prefix/lib directories.

        ldconfig

5. Using libdll

    test/dll_testcase.c should give you a pretty good impression of how to use
    the library. The header files dll_list.h and dll_util.h are reasonably well
    documented, too. 
    You should even be able to run doxygen against the code to produce some
    usable documentation.
