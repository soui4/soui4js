echo "copy bin for debug"
copy depends\quickjs\lib\Debug\x86\*.dll debug /y
copy depends\sdl2\bin\*.dll debug /y
copy depends\vodplayer\bin\*.dll debug /y
copy depends\soui4\bin\*.dll debug /y

echo "copy bin for debug"
copy depends\quickjs\lib\Release\x86\*.dll Release /y
copy depends\sdl2\bin\*.dll Release /y
copy depends\vodplayer\bin\*.dll Release /y
copy depends\soui4\bin\*.dll Release /y