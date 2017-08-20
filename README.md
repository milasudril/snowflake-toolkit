SnowflakeToolkit--A toolkit for generating ice particles
========================================================

Documentation
-------------
The documentation can be found within the thesis "A software toolkit for generating ice and snow particle sharp data", availible from http://studentarbeten.chalmers.se/publication/234473-a-software-toolkit-for-generating-ice-and-snow-particle-sharp-data. Additional documentation can be found on the GitHub wiki https://github.com/milasudril/snowflake-toolkit/wiki as well as in the changelog directory.


Dependencies
------------
The toolkit has been design with so it should not have to many external dependencies. But it relies on

 * glm - The "OpenGL Mathematics Library"
 * libhdf5-cpp + its dependencies
 * libjansson (libjansson-dev on Ubuntu)
 * GLEW (libglew-dev on Ubuntu)
 * GLFW (libglfw3-dev on Ubuntu)
 * OpenGL 3.3 (appropriate graphics driver)
 * libpng (libpng*-dev on Ubuntu)
 

Compiling
---------
This is a shell script that can be used to compile the toolkit. The toolkit requires support for C++14 or later.	

    #!/bin/bash

    # Download Maike [Any version newer than 0.10.6 should work, but see milasudril/maike#60. This means that versions newer than and including 0.10.8 may not work]
    wget https://github.com/milasudril/maike/archive/0.10.6.tar.gz

    # Rename the compressed tarball
    mv "0.10.6.tar.gz" "maike-0.10.6.tar.gz"

    # Decompress the tarball
    gzip -d "maike-0.10.6.tar.gz"

    # Extract its contents
    tar -xf "maike-0.10.6.tar.gz"

    # Cd into the Maike source directory
    cd maike-0.10.6

    # Compile Maike
    chmod u+x build.sh
    ./build.sh

    # Move Maike to an appropriate place
    mv __targets/maike ~/bin/maike
	mv __targets/*.so ~/bin

    # Leave the Gabi source directory
    cd ".."

    # Everything above this line can be skipped if an appropriate version
    # of Maike is already installed
    ##################################################################

    # Download the toolkit
    git clone "https://github.com/milasudril/snowflake-toolkit.git"

    # Cd into the snowflake toolkit directory
    cd "snowflake-toolkit/code"

    # Compile the code
    ./make-all.sh


License
-------
See the sub-directires for licensing information
