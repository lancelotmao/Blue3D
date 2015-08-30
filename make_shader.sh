#!/bin/bash
java -cp "/Users/lancemao/project/libhouyi/tools" OpenGLES_Shader_Exporter /Users/lancemao/project/libhouyi/shader/;
echo "Java done!";
mv -f "glshaders.h" "renderer/GLES20/include/glshaders.h";
mv -f "HIShaders.h" "renderer/GLES20/include/HIShaders.h";
mv -f "HIShaders.java" "src/com/dwtech/android/houyi/shader/HIShaders.java";
