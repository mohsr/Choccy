# Choccy: A Lisp language and interpreter written in C
Written by: Mohsin Rizvi

Last edited: 08/29/17

# *README*

## A. Overview

Choccy is a Lisp language and interpreter I am writing in C with the help
of Daniel Holden's guide at www.buildyourownlisp.com. Once I finish the
guide, I will continue to add new features to the language.

## B. Acknowledgments

As noted above, this project would probably not have been possible without
Daniel Holden's guide over at www.buildyourownlisp.com. The guide is
fantastically written, and, though I am not a beginner to programming, I feel
that a beginner would be comfortable (though sometimes challenged) using it.

Holden also provides a library he created called MPC to use alongside the
guide, making parsing input and representing the language much simpler. The
MPC library is located in the lib/mpc directory and is provided under the
BSD two-clause license (see lib/mpc/LICENSE.md).

## C. Files and Folders

Here is a brief overview of the files and folders included in the project.

  lib

    choccyparsing.c

      Contains the main source code for the Choccy interpreter,
      including function definitions.

    choccyparsing.h

      Header file for the main Choccy interpreter, including function
      declarations and data structure definitions.

    mpc

      LICENSE

        Contains the license for the MPC library, created by Daniel Holden.
        MPC uses a BSD two-clause license.

      mpc.c

        Contains source code for the MPC library, including function
        definitions. Provided by Daniel Holden under the BSD two-clause
        license.

      mpc.h

        Header file for the MPC library, including function declarations and
        data structure definitions.

  LICENSE

    Contains the license for Choccy and its interpreter. Choccy uses the
    MIT license.

  README.md

    Contains general information on Choccy. You're reading this!
