#version 330 core

layout(points) in;
layout (triangle_strip, max_vertices = 3) out;


void main(){
    gl_Position = gl_in[0].gl_Position + vec4(-0.005, 0.0, 0, 0.0); 
    // gl_Position.z=-1;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.0, 0.01, 0.0, 0.0);
    // gl_Position.z=1;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4( 0.005, 0.0, 0.0, 0.0);
    // gl_Position.z=1;
    EmitVertex();

    EndPrimitive();

}
