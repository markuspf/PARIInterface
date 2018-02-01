#
# PARIInterface: Interface to PARI
#
# Reading the declaration part of the package.
#
_PATH_SO:=Filename(DirectoriesPackagePrograms("PARIInterface"), "PARIInterface.so");
if _PATH_SO <> fail then
    LoadDynamicModule(_PATH_SO);
fi;
Unbind(_PATH_SO);

ReadPackage( "PARIInterface", "gap/PARIInterface.gd");
