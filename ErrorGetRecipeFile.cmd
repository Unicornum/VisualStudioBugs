@echo off

set MSBuild="%ProgramFiles% (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"

%MSBuild% ErrorGetRecipeFile.sln /t:BuildAll:Rebuild /p:Configuration=Debug;Platform=x64

pause
