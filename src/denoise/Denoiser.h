#pragma once
#include "Singleton.h"
#include <GL/gl3w.h>

namespace oidn
{
	class FilterRef;
	class DeviceRef;
}
namespace GLSLPT
{
	struct iVec2;
	struct Vec3;
}
class Denoiser:public Singleton<Denoiser>
{
public:
	Denoiser();
	void Denoise(GLuint& input, GLuint& output, GLSLPT::Vec3*& inPtr, GLSLPT::Vec3*& outPtr, GLSLPT::iVec2& size);
private:
	std::shared_ptr<oidn::FilterRef> filter;
	std::shared_ptr<oidn::DeviceRef> device;
};