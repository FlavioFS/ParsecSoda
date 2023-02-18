@REM Creating directories...
MKDIR .\Debug
MKDIR .\Debug\icons
MKDIR .\Debug\fonts
MKDIR .\Debug\sfx

MKDIR .\Release
MKDIR .\Release\icons
MKDIR .\Release\fonts
MKDIR .\Release\sfx

MKDIR .\x64\Debug
MKDIR .\x64\Debug\icons
MKDIR .\x64\Debug\fonts
MKDIR .\x64\Debug\sfx

MKDIR .\x64\Release
MKDIR .\x64\Release\icons
MKDIR .\x64\Release\fonts
MKDIR .\x64\Release\sfx


@REM Copying dependencies...
COPY .\Dependencies\parsecsdk\windows\parsec.dll .\x64\Debug
COPY .\Dependencies\parsecsdk\windows\parsec.dll .\x64\Release
COPY .\Dependencies\parsecsdk\windows\parsec32.dll .\Debug
COPY .\Dependencies\parsecsdk\windows\parsec32.dll .\Release

COPY .\Dependencies\sdl\lib\x64\SDL2.dll .\x64\Debug
COPY .\Dependencies\sdl\lib\x64\SDL2.dll .\x64\Release
COPY .\Dependencies\sdl\lib\x86\SDL2.dll .\Debug
COPY .\Dependencies\sdl\lib\x86\SDL2.dll .\Release

COPY .\ParsecSoda\fonts\ .\x64\Debug\fonts\
COPY .\ParsecSoda\fonts\ .\x64\Release\fonts\
COPY .\ParsecSoda\fonts\ .\Debug\fonts\
COPY .\ParsecSoda\fonts\ .\Release\fonts\

COPY .\ParsecSoda\sfx\ .\x64\Debug\sfx\
COPY .\ParsecSoda\sfx\ .\x64\Release\sfx\
COPY .\ParsecSoda\sfx\ .\Debug\sfx\

COPY .\ParsecSoda\sfx\ .\Release\sfx\
COPY .\ParsecSoda\icons\ .\x64\Debug\icons\
COPY .\ParsecSoda\icons\ .\x64\Release\icons\
COPY .\ParsecSoda\icons\ .\Debug\icons\
COPY .\ParsecSoda\icons\ .\Release\icons\

pause