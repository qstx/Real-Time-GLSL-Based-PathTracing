#version 430
#include common/uniforms.glsl

out vec4 color;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D colorTex;
layout (binding = 1) uniform sampler2D normalTex;
layout (binding = 2) uniform sampler2D positionTex;
uniform float sigmaP;
uniform float sigmaC;
uniform float sigmaN;
uniform float sigmaD;
uniform int kernelSize;

void main()
{
    color = vec4(0);
    float deltaX = 1.0f / resolution.x;
    float deltaY = 1.0f / resolution.y;
    float sumW = 0;
    vec3 normal = texture2D(normalTex, TexCoords).xyz;
    //vec3 position = texture2D(positionTex, TexCoords).xyz;
    float depth = texture2D(positionTex, TexCoords).w;

    for(int i=0;i< kernelSize;++i)
        for (int j = 0; j < kernelSize; ++j)
        {
            vec2 filterTexCoords = TexCoords + vec2((i - kernelSize/2) * deltaX, (j - kernelSize/2) * deltaY);
            vec3 normal2 = texture2D(normalTex, filterTexCoords).xyz;
            //vec3 position2 = texture2D(positionTex, filterTexCoords).xyz;
            float depth2 = texture2D(positionTex, filterTexCoords).w;
            float dNormal = acos(dot(normal2, normal));
            dNormal = max(0, dNormal);
            //float dPlane = dot(normal, normalize(position2 - position));
            float w =  exp(0
                -((i - kernelSize/2) * (i - kernelSize / 2) + (j - kernelSize / 2) * (j - kernelSize / 2)) / (2 * sigmaP * sigmaP)
                -(dNormal * dNormal) / (2 * sigmaN * sigmaN)
                //-(dPlane* dPlane)/ (2 * sigmaD * sigmaD)
                -(depth-depth2)* (depth - depth2)/(2* sigmaD * sigmaD)
            );
            color += w * texture2D(colorTex, filterTexCoords);
            sumW += w;
        }
    
    //color = vec4((normal+vec3(1))*0.5f,1);
    color = color/sumW;
    //color = texture(colorTex, TexCoords);
}