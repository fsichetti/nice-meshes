#version 460
layout(triangles) in;
layout(line_strip, max_vertices=4) out;

const float zoff = -0.01;

void main() {
    gl_Position = gl_in[0].gl_Position;
    gl_Position.z += zoff;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    gl_Position.z += zoff;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gl_Position.z += zoff;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position;
    gl_Position.z += zoff;
    EmitVertex();

    EndPrimitive();
}