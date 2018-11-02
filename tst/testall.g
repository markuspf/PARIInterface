LoadPackage( "PariInterface" );

TestDirectory( DirectoriesPackageLibrary("PariInterface", "tst"),
            rec(exitGAP := true, testOptions := rec(compareFunction := "uptowhitespace") ) );

# Should never get here
FORCE_QUIT_GAP(1);
