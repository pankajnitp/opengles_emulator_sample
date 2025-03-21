/*
 * shader.c
 * Functions for loading and process shaders.
 */

#include "shader.h"
#include "scene.h"

/* 
 * Loads the shader source into memory.
 *
 * sFilename: String holding filename to load 
 */
char* load_shader(char *sFilename) {
    char *pResult = NULL;
    FILE *pFile = NULL;
    long iLen = 0;

    pFile = fopen(sFilename, "r");

    if(pFile == NULL) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", sFilename);
  		exit(-1);
    }

    fseek(pFile, 0, SEEK_END); /* Seek end of file */
    iLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET); /* Seek start of file again */
    pResult = calloc(iLen+1, sizeof(char));
    fread(pResult, sizeof(char), iLen, pFile);
    pResult[iLen] = '\0';
    fclose(pFile);

    return pResult;
}

/* 
 * Create shader, load in source, compile, dump debug as necessary.
 *
 * pShader: Pointer to return created shader ID.
 * sFilename: Passed-in filename from which to load shader source.
 * iShaderType: Passed to GL, e.g. GL_VERTEX_SHADER.
 */
void process_shader(GLuint *pShader, char *sFilename, GLint iShaderType) {
	GLint iStatus;
	const char *aStrings[1] = { NULL };

	/* Create shader and load into GL. */
	*pShader = GL_CHECK(glCreateShader(iShaderType));
	
	aStrings[0] = load_shader(sFilename);
	
	GL_CHECK(glShaderSource(*pShader, 1, aStrings, NULL));

	/* Clean up shader source. */
	free((void *)aStrings[0]);
	aStrings[0] = NULL;

	/* Try compiling the shader. */
	GL_CHECK(glCompileShader(*pShader));
	GL_CHECK(glGetShaderiv(*pShader, GL_COMPILE_STATUS, &iStatus));

  if (!iStatus)
  {
    char log[512];
    glGetShaderInfoLog(*pShader, 512, NULL, log);
    printf("Shader Compilation Failed: %s\n", log);
  }
	// Dump debug info (source and log) if compilation failed.
	if(iStatus != GL_TRUE) {
		exit(-1);
	}
}

