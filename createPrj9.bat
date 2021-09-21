echo :MMP: %1
@call bldmake bldfiles 1>>c:\progettiincorso\warnings.txt
@call abld reallyclean gcce %1 1>NUL 2>NUL
@call abld reallyclean winscw %1 1>NUL 2>NUL
