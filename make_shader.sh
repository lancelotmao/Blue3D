#!/bin/bash
java -cp "/Users/lance/Project/Blue3D/tools" OpenGLES_Shader_Exporter /Users/lance/Project/Blue3D/Houyi/shader/;
echo "Java done!";
mv -f "glshaders.h" "Houyi/renderer/GLES20/include/glshaders.h";
mv -f "HIShaders.h" "Houyi/renderer/GLES20/include/HIShaders.h";
mv -f "HIShaders.java" "Houyi/src/main/java/com/dwtech/android/houyi/shader/HIShaders.java";
