@call abld reallyclean wins 1>NUL 2>NUL
@call abld reallyclean armi 1>NUL 2>NUL

@call abld build wins udeb 1>>c:\online\warnings.txt
@call abld build armi urel 1>>c:\online\warnings.txt 
