mkdir bin
copy Release\dll.dll bin\TBTray.dll
copy Release\loader.exe bin\TBTray.exe
copy README.md bin
copy LICENSE.txt bin
echo TBTray register > bin\register.cmd
echo TBTray unregister > bin\unregister.cmd
pause