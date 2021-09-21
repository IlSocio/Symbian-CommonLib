@echo :DLL: %1
del *.def 1>NUL 2>NUL
del ..\bwins\*.def 1>NUL 2>NUL
del ..\eabi\*.def 1>NUL 2>NUL

call comp9.bat %1

call abld freeze gcce %1 1>>c:\progettiincorso\warnings.txt
call abld freeze winscw %1 1>>c:\progettiincorso\warnings.txt

call createprj9.bat %1

call comp9.bat %1
