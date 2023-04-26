@REM Register COM server
echo PWD=%cd%
echo Register server %1
del %2
@REM runas /Administrators 
regsvr32 /s /u "%1" && echo Unregistred
@REM runas /Administrators
regsvr32 /s    "%1" && echo Registered && echo OK>%2
