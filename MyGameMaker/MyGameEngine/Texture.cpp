#include "Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "Renderer.h"

Texture::Texture(const string& path, unsigned int* valor) : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
m_Width(0), m_Height(0), m_BPP(0)
{
	auto il_img_id = ilGenImage();
	ilLoadImage((const wchar_t*)path.c_str());

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	const auto origin = ilGetInteger(IL_IMAGE_ORIGIN);
	if (origin != IL_ORIGIN_UPPER_LEFT) iluFlipImage();

	// Generar Mipmap y configurar parámetros de textura
	glGenerateMipmap(GL_TEXTURE_2D);
	//GL_TEXTURE_MIN_FILTER per si la imatge es mes gran que el pla on es pinta i GL_TEXTURE_MAG_FILTER pel contrari
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//S = X, T = Y (entiendo que es para que cuadre el x = 0 i y = 0 en las mismas del plano
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// Free image
	ilDeleteImage(il_img_id);

	if (valor != nullptr)
	{
		*valor = m_RendererID;
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
