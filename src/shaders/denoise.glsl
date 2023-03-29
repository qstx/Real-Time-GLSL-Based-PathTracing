#version 430
#include common/uniforms.glsl
#include common/globals.glsl

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 debugColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D colorTex;
layout (binding = 1) uniform sampler2D normalTex;
layout (binding = 2) uniform sampler2D positionTex;
layout (binding = 3) uniform sampler2D olderColorTex;
uniform float sigmaP;
uniform float sigmaC;
uniform float sigmaN;
uniform float sigmaD;
uniform int kernelSize;
uniform Camera lastCamera;

void main()
{
    outColor = vec4(0);
    float deltaX = 1.0f / resolution.x;
    float deltaY = 1.0f / resolution.y;
    float sumW = 0;
    vec3 normal = texture2D(normalTex, TexCoords).xyz;
    vec3 color = texture2D(colorTex, TexCoords).xyz;
    vec3 position = texture2D(positionTex, TexCoords).xyz;
    float depth = texture2D(positionTex, TexCoords).w;

    for(int i=0;i< kernelSize;++i)
        for (int j = 0; j < kernelSize; ++j)
        {
            vec2 filterTexCoords = TexCoords + vec2((i - kernelSize/2) * deltaX, (j - kernelSize/2) * deltaY);
            vec3 normal2 = texture2D(normalTex, filterTexCoords).xyz;
            vec3 color2 = texture2D(colorTex, filterTexCoords).xyz;
            //vec3 position2 = texture2D(positionTex, filterTexCoords).xyz;
            float depth2 = texture2D(positionTex, filterTexCoords).w;
            float dNormal = max(acos(dot(normal2, normal)),0);
            if (length(normal) < 0.1f || length(normal2) < 0.1f)
                dNormal = 3.14;
            //float dPlane = dot(normal, normalize(position2 - position));
            float w =  exp(0
                -((i - kernelSize/2) * (i - kernelSize / 2) + (j - kernelSize / 2) * (j - kernelSize / 2)) / (2 * sigmaP * sigmaP)
                -pow(length(color2-color),2)/(2*sigmaC* sigmaC)
                -(dNormal * dNormal) / (2 * sigmaN * sigmaN)
                //-(dPlane* dPlane)/ (2 * sigmaD * sigmaD)
                -(depth-depth2)* (depth - depth2)/(2* sigmaD * sigmaD)
            );
            outColor += w * texture2D(colorTex, filterTexCoords);
            sumW += w;
        }
    
    vec3 rayDirInCamera=vec3(
        dot(position - lastCamera.position, lastCamera.right),
        dot(position - lastCamera.position, lastCamera.up),
        dot(position - lastCamera.position, lastCamera.forward)
        );
    rayDirInCamera = rayDirInCamera / rayDirInCamera.z*(resolution.x * 0.5f /tan(lastCamera.fov*0.5f));
    vec2 motionVec = rayDirInCamera.xy / resolution + vec2(0.5f)- TexCoords;
    vec2 lastTexCoords = rayDirInCamera.xy / resolution + vec2(0.5f);
    if (lastTexCoords.x >= 0 && lastTexCoords.y >= 0 && lastTexCoords.x <= 1 && lastTexCoords.y <= 1)
    {
        vec4 lastColor = texture2D(olderColorTex, lastTexCoords);
        outColor = outColor / sumW;
        outColor = outColor * 0.2f + lastColor * 0.8f;
        debugColor = vec4(motionVec*5,0,1);
    }
    else
    {
        outColor = outColor / sumW;
    }

    //outColor = texture2D(olderColorTex, TexCoords)+vec4(0.001f,0,0,0);
}