#version 330 core
layout (triangles) in; //输入三角形图元
layout (line_strip, max_vertices = 6) out; //输出线段图元

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.01;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();//线段起点
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();//线段终点
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}
