#include "Denoiser.h"
#include <iostream>
#include "OpenImageDenoise\oidn.hpp"
#include "Vec3.h"
#include "Vec2.h"

void Denoiser::Denoise(GLuint &input,  GLuint &output, GLSLPT::Vec3* &inPtr, GLSLPT::Vec3* &outPtr, GLSLPT::iVec2 &size)
{
	// FIXME: Figure out a way to have transparency with denoiser
	glBindTexture(GL_TEXTURE_2D, input);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, (GLvoid*)inPtr);

	filter->setImage("color", inPtr, oidn::Format::Float3, size.x, size.y, 0, 0, 0);
	filter->setImage("output", outPtr, oidn::Format::Float3, size.x, size.y, 0, 0, 0);
	filter->commit();

	// Filter the image
	filter->execute();

	// Check for errors
	const char* errorMessage;
	if (device->getError(errorMessage) != oidn::Error::None)
		std::cout << "Error: " << errorMessage << std::endl;

	// Copy the denoised data to denoisedTexture
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x, size.y, 0, GL_RGB, GL_FLOAT, outPtr);
}

Denoiser::Denoiser()
{
	device = std::make_shared<oidn::DeviceRef>(oidn::newDevice());
	//device->set("numThreads", 10);
	device->commit();
	filter = std::make_shared<oidn::FilterRef>(device->newFilter("RT"));
	filter->set("hdr", false);
	filter->commit();
}