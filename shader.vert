/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2011 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

attribute vec3 av4position;
attribute vec2 a_texCoord;

uniform mat4 mvp;

//varying vec3 vv3colour;
varying vec2 v_texCoord;

void main() {
	//vv3colour = av3colour;
	//gl_Position = mvp * av4position;//
	gl_Position = vec4(av4position, 1.0);
	v_texCoord = a_texCoord;
}

