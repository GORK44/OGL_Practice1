#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    if(hdr)
    {
        // Reinhard色调映射
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        
        // exposure 曝光色调映射。exposure越小，明亮区域细节越丰富；exposure越大，黑暗部分细节越丰富
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);// e^(-hdrColor * exposure)
        
        
        // gamma矫正 also gamma correct while we're at it
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}
