@echo :DLL: %1
@del winscw.def 1>NUL 2>NUL

@call comp9win.bat %1

@call abld freeze winscw %1 1>>c:\online\warnings.txt

@call createprj9win.bat %1

@call comp9win.bat %1
