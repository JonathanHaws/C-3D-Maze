:: for metadata on windows app like icon, description, and creator you have to bake it into the executable instead of just reading it from local file. 
:: This is done by creating a resource script and compiling it with the resource compiler.
:: Then the rc file has to be linked with the compiler.
:: I know windows is ancient and has a lot of bloat and legacy, but this is masochistic.
:: I wish .exe was just an allias for a folder not the binary. Would make life so much easier.

:rc.exe /fo metadata.res metadata.rc