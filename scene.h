#ifndef CUBE_H
#define CUBE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

typedef struct {
	float x, y, z;
} vec3;

void ApplyEffect(int on, int obj);

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(1); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __LINE__); \
            exit(1); \
        } \
    }

#endif /* CUBE_H */
