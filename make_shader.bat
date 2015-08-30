@echo off
set DIR_ROOT=%cd%

echo Exporting shaders
cd tools
java OpenGLES_Shader_Exporter %DIR_ROOT%\shader
echo ======= Exporting shaders Start =======
copy /Y  *.h ..\renderer\GLES20\include
copy /Y HIShaders.java ..\src\com\dwtech\android\houyi\shader
xcopy /E /Y /Q %DIR_ROOT%\shader ..\demo\assets\shader
echo ======= Exporting shaders done! =======

cd..

