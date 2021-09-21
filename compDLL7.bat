echo :DLL: %1

@call comp7.bat

@call abld freeze winscw 1>>c:\online\warnings.txt
@call abld freeze armi 1>>c:\online\warnings.txt

@call createprj7.bat %1

@call comp7.bat
