#include "Material.h"

//a inclure en dernier
#include "DebugEngine/DebugNew.h"

namespace GameEngine
{
	Material::Material()
	{
	  // Set a default material (white)
	  ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));
	  SetDiffuseColor(255,255,255);
	  SetAmbientColor(255,255,255);
	  SetSpecularColor(255,255,255);
	  SetEmissiveColor(0,0,0);
	  SetPower(1.0f);
	}

	bool Material::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
	  m_Material.Diffuse.r = 1.0f / 255.0f * (float)Red;
	  m_Material.Diffuse.g = 1.0f / 255.0f * (float)Green;
	  m_Material.Diffuse.b = 1.0f / 255.0f * (float)Blue;

	  return true;
	}

	bool Material::GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
	{
	  if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Diffuse.r);

	  if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Diffuse.g);

	  if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Diffuse.b);

	  return true;
	}

	bool Material::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
	  m_Material.Ambient.r = 1.0f / 255.0f * (float)Red;
	  m_Material.Ambient.g = 1.0f / 255.0f * (float)Green;
	  m_Material.Ambient.b = 1.0f / 255.0f * (float)Blue;

	  return true;
	}

	bool Material::GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
	{
	  if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Ambient.r);

	  if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Ambient.g);

	  if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Ambient.b);

	  return true;
	}

	bool Material::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
	  m_Material.Specular.r = 1.0f / 255.0f * (float)Red;
	  m_Material.Specular.g = 1.0f / 255.0f * (float)Green;
	  m_Material.Specular.b = 1.0f / 255.0f * (float)Blue;

	  return true;
	}

	bool Material::GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
	{
	  if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Specular.r);

	  if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Specular.g);

	  if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Specular.b);

	  return true;
	}

	bool Material::SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
	  m_Material.Emissive.r = 1.0f / 255.0f * (float)Red;
	  m_Material.Emissive.g = 1.0f / 255.0f * (float)Green;
	  m_Material.Emissive.b = 1.0f / 255.0f * (float)Blue;

	  return true;
	}

	bool Material::GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
	{
	  if(Red != NULL)
		*Red = (unsigned char)(255.0f * m_Material.Emissive.r);

	  if(Green != NULL)
		*Green = (unsigned char)(255.0f * m_Material.Emissive.g);

	  if(Blue != NULL)
		*Blue = (unsigned char)(255.0f * m_Material.Emissive.b);

	  return true;
	}

	bool Material::SetPower(float Power)
	{
	  m_Material.Power = Power;
	  return true;
	}

	float Material::GetPower(float Power)
	{
	  return m_Material.Power;
	}

	D3DMATERIAL9 *Material::GetMaterial()
	{
	  return &m_Material;
	}
} // namespace GameEngine