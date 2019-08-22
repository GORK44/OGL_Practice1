#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;//sin值【-1，1】变换到了【0, 1】
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b)); //叉乘，得到三角形法线（注意叉乘顺序）
}

void main() {
    vec3 normal = GetNormal();
    
    gl_Position = explode(gl_in[0].gl_Position, normal);//三角形图元第 1 个点位移
    TexCoords = gs_in[0].texCoords;//三角形第 1 个点的uv
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);//三角形图元第 2 个点位移
    TexCoords = gs_in[1].texCoords;//三角形第 2 个点的uv
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);//三角形图元第 3 个点位移
    TexCoords = gs_in[2].texCoords;//三角形第 3 个点的uv
    EmitVertex();
    
    EndPrimitive();
}
