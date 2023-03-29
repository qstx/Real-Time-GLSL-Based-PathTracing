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

#pragma once

#include <vector>
#include "Quad.h"
#include "Program.h"
#include "Vec2.h"
#include "Vec3.h"

namespace GLSLPT
{
    Program* LoadShaders(const ShaderInclude::ShaderSource& vertShaderObj, const ShaderInclude::ShaderSource& fragShaderObj);

    struct RenderOptions
    {
        RenderOptions()
        {
            renderResolution = iVec2(1280, 720);
            windowResolution = iVec2(1280, 720);
            uniformLightCol = Vec3(0.3f, 0.3f, 0.3f);
            backgroundCol = Vec3(1.0f, 1.0f, 1.0f);
            tileWidth = 100;
            tileHeight = 100;
            maxDepth = 2;
            maxSpp = -1;
            RRDepth = 2;
            texArrayWidth = 2048;
            texArrayHeight = 2048;
            denoiserFrameCnt = 20;
            enableRR = true;
            enableDenoiser = false;
            enableTonemap = false;
            enableAces = false;
            openglNormalMap = true;
            enableEnvMap = false;
            enableUniformLight = false;
            hideEmitters = false;
            enableBackground = false;
            transparentBackground = false;
            independentRenderSize = false;
            enableRoughnessMollification = false;
            enableVolumeMIS = false;
            envMapIntensity = 1.0f;
            envMapRot = 0.0f;
            roughnessMollificationAmt = 0.0f;

            sigmaP = 6.83;
            sigmaC = 0.3;
            sigmaD = 0.048;
            sigmaN = 0.62;
            kernelSize = 33;
        }

        iVec2 renderResolution;
        iVec2 windowResolution;
        Vec3 uniformLightCol;
        Vec3 backgroundCol;
        int tileWidth;
        int tileHeight;
        int maxDepth;
        int maxSpp;
        int RRDepth;
        int texArrayWidth;
        int texArrayHeight;
        int denoiserFrameCnt;
        bool enableRR;
        bool enableDenoiser;
        bool enableTonemap;
        bool enableAces;
        bool simpleAcesFit;
        bool openglNormalMap;
        bool enableEnvMap;
        bool enableUniformLight;
        bool hideEmitters;
        bool enableBackground;
        bool transparentBackground;
        bool independentRenderSize;//屏幕尺寸和渲染尺寸是否独立
        bool enableRoughnessMollification;
        bool enableVolumeMIS;
        float envMapIntensity;
        float envMapRot;
        float roughnessMollificationAmt;

        float sigmaP;
        float sigmaC;
        float sigmaD;
        float sigmaN;
        int kernelSize;
    };

    class Scene;

    class Renderer
    {
    protected:
        Scene* scene;
        Quad* quad;

        // Opengl buffer objects and textures for storing scene data on the GPU
        GLuint BVHBuffer;
        GLuint BVHTex;
        GLuint vertexIndicesBuffer;
        GLuint vertexIndicesTex;
        GLuint verticesBuffer;
        GLuint verticesTex;
        GLuint normalsBuffer;
        GLuint normalsTex;
        GLuint materialsTex;
        GLuint transformsTex;
        GLuint lightsTex;
        GLuint textureMapsArrayTex;
        GLuint envMapTex;
        GLuint envMapCDFTex;

        // FBOs
        GLuint pathTraceFBO;
        GLuint denoiseFBO;
        GLuint copyFBO;

        // Shaders
        std::string shadersDirectory;
        Program* pathTraceShader;//链接了vertex.glsl和preview.glsl
        Program* denoiseShader;//链接了vertex.glsl和denoise.glsl
        Program* tonemapShader;//链接了vertex.glsl和tonemap.glsl
        Program* copyShader;//链接了vertex.glsl和output.glsl

        // Render textures
        GLuint pathTraceTexture[2];//pathTraceFBOLowRes的颜色附件
        GLuint gNormalTexture;//GBuffer中法线
        GLuint gPositionTexture;//GBuffer中法线
        GLuint denoiseTexture;
        GLuint denoiseDebugTexture;

        // Render resolution and window resolution
        iVec2 renderSize;
        iVec2 windowSize;

        // Variables to track rendering status
        int currentPathTraceOutput;
        iVec2 tile;
        iVec2 numTiles;
        Vec2 invNumTiles;
        int tileWidth;
        int tileHeight;
        int currentBuffer;
        int frameCounter;
        int sampleCounter;

        bool initialized;

    public:
        Renderer(Scene* scene, const std::string& shadersDirectory);
        ~Renderer();

        void ResizeRenderer();
        void ReloadShaders();
        //根据场景是否发生改变或者是否到达maxSpp来决定是否渲染
        void Render();
        //根据设置选取合适Shader绘制默认帧缓冲对象
        void Present();
        //根据场景是否发生改变或者是否到达maxSpp来决定是否更新渲染数据
        void Update(float secondsElapsed);
        //主要用于记录当前帧的相机状态，供下一帧计算motion vector
        void PostUpdate();
        float GetProgress();
        int GetSampleCount();
        void GetOutputBuffer(unsigned char**, int& w, int& h);

    private:
        void InitGPUDataBuffers();
        //初始化符合需要的帧缓冲对象
        void InitFBOs();
        //初始化Shader对象
        void InitShaders();
    };
}