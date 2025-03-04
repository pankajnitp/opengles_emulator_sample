
#ifndef SHADER_H
#define SHADER_H

#include "GLES2/gl2.h"

char* load_shader(char *sFilename);
void process_shader(GLuint *pShader, char *sFilename, GLint iShaderType);

#endif

