echo :MMP: %1
REM @call makmake %1.mmp vs2003 1>>c:\online\warnings.txt
@call bldmake bldfiles 1>>c:\online\warnings.txt
@call abld reallyclean winscw %1 1>NUL 2>NUL
