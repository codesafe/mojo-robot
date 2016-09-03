echo Restart Robot
ping 127.0.0.1 -n 3 > nul
start /d ".\" /b robotrun_2015.exe
