#version 330 core
layout (points) in; //从顶点着色器输入的图元类型
layout (triangle_strip, max_vertices = 5) out; //几何着色器输出的图元类型

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left
    EmitVertex(); //调用EmitVertex时，gl_Position中的向量会被添加到图元中来
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0); // 5:top
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive(); //所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元
}

void main() {
    build_house(gl_in[0].gl_Position);
}
