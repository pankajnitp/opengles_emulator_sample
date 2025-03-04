#include "scene.h"
#include "shader.h"
#include "window.h"
#include "matrix.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

HWND hWindow;
HDC  hDisplay;
int runTransition = 0;
/* 3D data. Vertex range -0.5..0.5 in all axes.
* Z -0.5 is near, 0.5 is far. */
const float aVertices[] =
{
    /* Front face. */
    /* Bottom left */
    -1.0,  -1.0, 0.0,
    1.0, 1.0, 0.0,
    -1.0, 1.0, 0.0,
    /* Top right */
    -1.0,  -1.0, 0.0,
    1.0,  -1.0, 0.0,
    1.0, 1.0, 0.0,
};

const float aCoords[] =
{
    0.0, 1.0,
    1.0, 0.0,
    0.0, 0.0,

    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0,
};


GLuint loadTexture(const char* imagePath) {
  int width, height, nrChannels;
  unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 4);

  if (!data) {
    printf("Failed to load image: %s\n", imagePath);
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  stbi_image_free(data);
  return texture;
}

int main(int argc, char **argv) {
    EGLDisplay	sEGLDisplay;
    EGLContext	sEGLContext;
    EGLSurface	sEGLSurface;

    /* EGL Configuration */

    EGLint aEGLAttributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLint aEGLContextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLConfig	aEGLConfigs[1];
    EGLint		cEGLConfigs;

    MSG sMessage;

    GLint iLocPosition = 0;
    GLint iLocTex, iLocMVP;
    GLuint uiProgram, uiFragShader, uiVertShader;
    int bDone = 0;
    static int nCount = 0;
    const unsigned int uiWidth  = 1920;
    const unsigned int uiHeight = 1080;
    float curve_fac = 0.0;

    int iXangle = 0, iYangle = 0, iZangle = 0;

    float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.

    unsigned char *myPixels = calloc(1, 128*128*4); // Holds texture data.
    unsigned char *myPixels2 = calloc(1, 128*128*4); // Holds texture data.

    float aRotate[16], aModelView[16], aPerspective[16], aMVP[16];

    hDisplay = EGL_DEFAULT_DISPLAY;

    sEGLDisplay = EGL_CHECK(eglGetDisplay(hDisplay));

    EGL_CHECK(eglInitialize(sEGLDisplay, NULL, NULL));
    EGL_CHECK(eglChooseConfig(sEGLDisplay, aEGLAttributes, aEGLConfigs, 1, &cEGLConfigs));

    if (cEGLConfigs == 0) {
        printf("No EGL configurations were returned.\n");
        exit(-1);
    }

    hWindow = create_window(uiWidth, uiHeight);

    sEGLSurface = EGL_CHECK(eglCreateWindowSurface(sEGLDisplay, 
		aEGLConfigs[0], (EGLNativeWindowType)hWindow, NULL));

    if (sEGLSurface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface.\n");
        exit(-1);
    }

    sEGLContext = EGL_CHECK(eglCreateContext(sEGLDisplay, 
		aEGLConfigs[0], EGL_NO_CONTEXT, aEGLContextAttributes));

    if (sEGLContext == EGL_NO_CONTEXT) {
        printf("Failed to create EGL context.\n");
        exit(-1);
    }

    EGL_CHECK(eglMakeCurrent(sEGLDisplay, sEGLSurface, sEGLSurface, sEGLContext));

    /* Shader Initialisation */
    process_shader(&uiVertShader, "shader.vert", GL_VERTEX_SHADER);
    process_shader(&uiFragShader, "shader.frag", GL_FRAGMENT_SHADER);

    /* Create uiProgram (ready to attach shaders) */
    uiProgram = GL_CHECK(glCreateProgram());

    /* Attach shaders and link uiProgram */
    GL_CHECK(glAttachShader(uiProgram, uiVertShader));
    GL_CHECK(glAttachShader(uiProgram, uiFragShader));
    GL_CHECK(glLinkProgram(uiProgram));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    iLocPosition = GL_CHECK(glGetAttribLocation(uiProgram, "av4position"));
    iLocTex = GL_CHECK(glGetAttribLocation(uiProgram, "a_texCoord"));

#ifdef DEBUG
    printf("iLocPosition = %i\n", iLocPosition);
    printf("iLocTex   = %i\n", iLocTex);
#endif

    /* Get uniform locations */
    iLocMVP = GL_CHECK(glGetUniformLocation(uiProgram, "mvp"));

#ifdef DEBUG
    printf("iLocMVP      = %i\n", iLocMVP);
#endif

    GL_CHECK(glUseProgram(uiProgram));

    loadTexture("image.jpg");
    /* Enable attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glEnableVertexAttribArray(iLocPosition));
    GL_CHECK(glEnableVertexAttribArray(iLocTex));

    /* Populate attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, aVertices));
    GL_CHECK(glVertexAttribPointer(iLocTex, 2, GL_FLOAT, GL_FALSE, 0, aCoords));

    //LoadTexture();

    //GL_CHECK(glEnable(GL_CULL_FACE));
    //GL_CHECK(glEnable(GL_DEPTH_TEST));

    /* Enter event loop */
    while (!bDone) {
        if(PeekMessage(&sMessage, NULL, 0, 0, PM_REMOVE)) {
            if(sMessage.message == WM_QUIT) {
                bDone = 1;
            } else {
                TranslateMessage(&sMessage);
                DispatchMessage(&sMessage);
            }
        }

        if (runTransition)
        {
          nCount++;
          float scale_fac = (nCount * 0.025);
          float trans_fac = nCount * 0.0833;
          curve_fac += 0.01;
          if (scale_fac > 0.75) scale_fac = 0.75;
          if (trans_fac > 2.5) trans_fac = 2.5;
          if (curve_fac > 0.25) curve_fac = 0.25;
          scale_matrix(0.25 + scale_fac, 0.25 + scale_fac, 1.0, aRotate);
          translate_matrix(2.5 - trans_fac, -2.5 + trans_fac, 0.0, aModelView);
          multiply_matrix(aRotate, aModelView, aMVP);
          GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP));
        }
        else
        {
          nCount = 0;
          scale_matrix(0.25, 0.25, 1.0, aRotate);
          translate_matrix(2.5, -2.5, 0.0, aModelView);
          multiply_matrix(aRotate, aModelView, aMVP);
          GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP));
        }


        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

        if (!eglSwapBuffers(sEGLDisplay, sEGLSurface)) {
            printf("Failed to swap buffers.\n");
        }

        Sleep(20);
    }

    /* Cleanup shaders */
    GL_CHECK(glUseProgram(0));
    GL_CHECK(glDeleteShader(uiVertShader));
    GL_CHECK(glDeleteShader(uiFragShader));
    GL_CHECK(glDeleteProgram(uiProgram));

    /* EGL clean up */
    EGL_CHECK(eglMakeCurrent(sEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    EGL_CHECK(eglDestroySurface(sEGLDisplay, sEGLSurface));
    EGL_CHECK(eglDestroyContext(sEGLDisplay, sEGLContext));
    EGL_CHECK(eglTerminate(sEGLDisplay));

    return 0;
}
