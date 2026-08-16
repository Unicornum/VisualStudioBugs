@echo off

set MSBuild="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\msbuild.exe"

%MSBuild% ErrorGetRecipeFile.sln /t:BuildAll:Rebuild /p:Configuration=Debug;Platform=x64

pause
