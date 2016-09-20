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
 * libhdf5-cpp + its dependencies

Compiling
--------------
This is a shell script that can be used to compile the toolkit

    #!/bin/bash

    # Download Maike [Any version newer than 0.7.2 should work]
    wget https://github.com/milasudril/maike/archive/0.7.2.tar.gz

    # Rename the compressed tarball
    mv "0.7.2.tar.gz" "maike-0.7.2.tar.gz"

    # Decompress the tarball
    gzip -d "maike-0.7.2.tar.gz"

    # Extract its contents
    tar -xf "maike-0.7.2.tar.gz"

    # Cd into the Maike source directory
    cd maike-0.7.2

    # Compile Maike
    chmod u+x build.sh
    ./build.sh

    # Move Maike to an appropriate place
    mv __targets/maike ~/bin/maike
	mv __targets/*.so ~/bin

    # Leave the Gabi source directory
    cd ".."

    # Everything above this line can be skipped if the correct version
    # of Maike is already installed
    ##################################################################

    # Download the toolkit
    git clone "https://github.com/milasudril/snowflake-toolkit.git"

    # Cd into the snowflake toolkit directory
    cd "snowflake-toolkit/code"

    # Run Wand to compile the project
    ~/bin/maike

Besides the release profile (stored in `release.json`), there a "profile", and a "debug" profile. See the JSON files for information about the compiler settings.

License
-------
See the sub-directires for licensing information
