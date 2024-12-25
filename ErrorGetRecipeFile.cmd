@echo off

set MSBuild="D:\Program Files\Visual Studio 2022\MSBuild\Current\Bin\amd64\msbuild.exe"

%MSBuild% ErrorGetRecipeFile.sln /t:BuildAll:Rebuild /p:Configuration=Debug;Platform=x64

pause
