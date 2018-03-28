#include "pch.h"
#include "teximage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool TexImage::Load(const std::string& filename, const std::string& base_dir)
{
	unsigned char* image = stbi_load(filename.c_str(), &m_w, &m_h, &m_bpp, STBI_default);
	if (!image)
	{
		std::cerr << "Unable to find file: " << filename << std::endl;
		std::string texfilename = base_dir + filename;
		if (!stbi_load(texfilename.c_str(), &m_w, &m_h, &m_bpp, STBI_default))
		{
			std::cerr << "failed to load texture: " << texfilename << std::endl;
			return false;
		}
	}
	std::cout << "Loaded texture: " << filename
		<< ", w = " << m_w
		<< ", h = " << m_h 
		<< ", comp = " << m_bpp << std::endl;

	m_data = (void *)image;
	return true;
}

void TexImage::Release()
{
	stbi_image_free(m_data);
}
