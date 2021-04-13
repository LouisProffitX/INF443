#version 330 core

layout(location=0)in vec3 aPos;

/*
out VS_OUT {
	
} vs_out;
*/

uniform mat4 perspective;
uniform mat4 view;

void main(){
	gl_Position=perspective*view*vec4(aPos,1.);
}