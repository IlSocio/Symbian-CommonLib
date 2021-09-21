call abld reallyclean winscw %1 1>NUL 2>NUL
call abld reallyclean gcce %1 1>NUL 2>NUL
call abld build winscw udeb %1 >> c:\progettiincorso\warnings.txt 
call abld build gcce urel %1 >> c:\progettiincorso\warnings.txt 
