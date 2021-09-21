@call abld reallyclean winscw %1 1>NUL 2>NUL
@call abld build winscw udeb %1 >> c:\online\warnings.txt 
