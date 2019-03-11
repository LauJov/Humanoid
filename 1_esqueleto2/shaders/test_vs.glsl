#version 410

layout(location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
uniform mat4 model, view, proj;

out vec3 position_eye, normal_eye;

// use z position to shader darker to help perception of distance

void main() {

	gl_Position = proj * view * model * vec4 (vertex_position, 1.0); //*1.0;

	position_eye = vec3 (view * model * vec4 (vertex_position, 1.0));
	normal_eye = vec3 (view * model * vec4 (vertex_normal, 0.0));


}
