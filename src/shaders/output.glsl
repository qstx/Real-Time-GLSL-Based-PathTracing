/*
 * MIT License
 *
 * Copyright(c) 2019 Asif Ali
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//在片元着色器阶段对某个Texture采样
#version 430
#include common/uniforms.glsl

out vec4 color;
in vec2 TexCoords;

uniform sampler2D imgTex;

void main()
{
    float deltaX = 1.0f / resolution.x;
    float deltaY = 1.0f / resolution.y;
    vec4 colorMean=vec4(0),color2Mean = vec4(0);
    for(int i=-3;i<=3;++i)
        for (int j = -3; j <= 3; ++j)
        {
            vec4 tmp = texture(imgTex, TexCoords+vec2(i* deltaX,j* deltaY));
            colorMean += tmp;
            color2Mean += tmp * tmp;
        }
    colorMean = colorMean / 49.0f;
    color2Mean = color2Mean / 49.0f;
    vec4 sigma = sqrt(color2Mean - colorMean * colorMean);
    color = texture(imgTex, TexCoords);
    color = clamp(color, colorMean - sigma, colorMean + sigma);
}