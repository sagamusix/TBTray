mkdir bin
copy README.md bin
copy LICENSE.txt bin

mkdir bin\32-bit
copy Win32\Release\dll.dll bin\32-bit\TBTray.dll
copy Win32\Release\loader.exe bin\32-bit\TBTray.exe
echo TBTray register > bin\32-bit\register.cmd
echo TBTray unregister > bin\32-bit\unregister.cmd

mkdir bin\64-bit
copy x64\Release\dll.dll bin\64-bit\TBTray.dll
copy x64\Release\loader.exe bin\64-bit\TBTray.exe
echo TBTray register > bin\64-bit\register.cmd
echo TBTray unregister > bin\64-bit\unregister.cmd

pause