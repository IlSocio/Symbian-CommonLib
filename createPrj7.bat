echo :MMP: %1
@call makmake %1.mmp vc6 1>>c:\online\warnings.txt
@call bldmake bldfiles 1>>c:\online\warnings.txt
@call abld reallyclean armi 1>NUL 2>NUL
@call abld reallyclean wins 1>NUL 2>NUL
