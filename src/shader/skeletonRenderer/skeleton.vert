#version 330 core
#define MAX_JOINT_MATRIX 128
#define MAX_MORPH 16


layout (location = 0) in vec3 a_pos;


uniform mat4 node;// specific node transform
uniform mat4 model;// global model transform
uniform mat4 view;// camera
uniform mat4 projection;// camera

void main()
{
    vec4 pos = vec4(a_pos,1.0);
    pos = projection * view * model * node * pos;
    gl_Position = pos;
}
