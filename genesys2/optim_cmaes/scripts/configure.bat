:: Create dir if not already exists
SET "actual_dir=%~dp0"

:: Cause variables will be expanded at execution time rather than at parse time
setlocal EnableDelayedExpansion

:: Read folder structure out of folders.txt
set i=0
for /F %%a in (Folders_Windows.txt) do (
   set /A i+=1
   set array[!i!]="%actual_dir% %%a"
)
set n=%i%

:: Create directory of folder if it does not exist already
for /L %%j in (1,1,%n%) do (
IF NOT EXIST !array[%%j]! (
MKDIR !array[%%j]!
)
)

:: Call Genesys.exe
echo "Start Genesys-application"
start /d "%actual_dir% Debug\" genesys_2.exe
:: print variable on a screen
ECHO "Done"​
