@setlocal
@echo off

rem VS post build step command line sample:
rem call postbuild.bat $(OutDir) $(Configuration) $(Platform) $(PlatformToolsetVersion)

set outdir=%1

if not "%TEAMCITY_PROJECT_NAME%" == "" (
    echo Running unit tests...
    call "%outdir%\disruptor.Tests.exe" --detect_memory_leaks=0 --result_code=no
)

@endlocal
