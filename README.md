SnowflakeToolkit--A toolkit for generating ice particles
========================================================

Documentation
-------------
The documentation can be found within the thesis "A software toolkit for generating ice and snow particle sharp data", availible from http://studentarbeten.chalmers.se/publication/234473-a-software-toolkit-for-generating-ice-and-snow-particle-sharp-data


Dependencies
------------
The toolkit has been design with so it should not have to many external
dependencies. But it relies on

 * glm - The "OpenGL Mathematics Library"

Compiling
--------------
This is a shell script that can be used to compile the toolkit

    #!/bin/bash

    # Download Gabi
    wget "https://github.com/milasudril/gabi/archive/5.77.tar.gz"

    # Rename the compressed tarball
    mv "5.77.tar.gz" "gabi-5.77.tar.gz"

    # Decompress the tarball
    gzip -d "gabi-5.77.tar.gz"

    # Extract its contents
    tar -xf "gabi-5.77.tar"

    # Cd into the Gabi source directory
    cd "gabi-5.77/source"

    # Compile Gabi
    make -f "Makefile-GNULinux64"

    # Move Wand to an appropriate place
    mv "__wand_targets-x86_64-gnulinux/wand/wand" ~/bin/wand

    # Leave the Gabi source directory
    cd "../.."

    # Everything above this line can be skipped if the correct version
    # of Wand is already installed
    ##################################################################

    # Download the toolkit
    git clone "https://github.com/milasudril/snowflake-toolkit.git"

    # Cd into the snowflake toolkit directory
    cd "snowflake-toolkit/code"

    # Run Wand to compile the project
    ~/bin/wand "profile[release]"

Besides the release profile, there is a "profile", and a "debug" profile.

License
-------
See the sub-directires for licensing information
