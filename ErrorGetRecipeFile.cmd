@echo off

set MSBuild="%ProgramFiles% (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\msbuild.exe"

%MSBuild% ErrorGetRecipeFile.sln /t:BuildAll:Rebuild /p:Configuration=Debug;Platform=x64

pause
