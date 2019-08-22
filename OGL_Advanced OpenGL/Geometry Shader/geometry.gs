#version 330 core
layout (points) in; //从顶点着色器输入的图元类型
layout (triangle_strip, max_vertices = 5) out; //几何着色器输出的图元类型：三角形带(Triangle Strip)（在第一个三角形绘制完之后，每个后续顶点将会在上一个三角形边上生成另一个三角形：每3个临近的顶点将会形成一个三角形）(1, 2, 3)、(2, 3, 4)、(3, 4, 5)和(4, 5, 6)

in VS_OUT {
    vec3 color;
} gs_in[];  //从顶点着色器发来输入数据总是会以数组的形式表示出来，即便我们现在只有一个顶点gs_in[0]。

out vec3 fColor;  //输出颜色向量

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
    fColor = vec3(1.0, 1.0, 1.0); //将最后一个顶点的颜色设置为白色
    EmitVertex();
    
    EndPrimitive(); //所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元
}

void main() {
    build_house(gl_in[0].gl_Position);
}
