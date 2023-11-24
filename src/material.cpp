#include "material.hpp"
#include "shader.hpp"

Material::Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
	: mShader(shader)
	, mTexture(texture)
{

}

Material::Material(const Material& other)
{
	mShader = other.mShader;
	mTexture = other.mTexture;

	mUniformInts = other.mUniformInts;
	mUniformFloats = other.mUniformFloats;
	mUniformFloat2s = other.mUniformFloat2s;
	mUniformFloat3s = other.mUniformFloat3s;
	mUniformFloat4s = other.mUniformFloat4s;
	mUniformMat3s = other.mUniformMat3s;
	mUniformMat4s = other.mUniformMat4s;
}

void Material::SetShader(std::shared_ptr<Shader> shader)
{
	if (shader)
	{
		mShader = shader;
	}
}

void Material::SetTexture(std::shared_ptr<Texture> texture)
{
	mTexture = texture;
}

void Material::UpdateShaderUniforms()
{
	if (mShader)
	{
		for (const auto& it : mUniformInts)
		{
			mShader->SetUniformInt(it.first, it.second);
		}
		for (const auto& it : mUniformFloats)
		{
			mShader->SetUniformFloat(it.first, it.second);
		}
		for (const auto& it : mUniformFloat2s)
		{
			mShader->SetUniformFloat2(it.first, it.second);
		}
		for (const auto& it : mUniformFloat3s)
		{
			mShader->SetUniformFloat3(it.first, it.second);
		}
		for (const auto& it : mUniformFloat4s)
		{
			mShader->SetUniformFloat4(it.first, it.second);
		}
		for (const auto& it : mUniformMat3s)
		{
			mShader->SetUniformMat3(it.first, it.second);
		}
		for (const auto& it : mUniformMat4s)
		{
			mShader->SetUniformMat4(it.first, it.second);
		}
	}
}