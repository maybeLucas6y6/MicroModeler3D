#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Shader
{
public:
	Shader(const std::string& vertexCode, const std::string& fragmentCode);
	~Shader();

	std::string GetVertexShaderSource() const { return mVertexShader; }
	std::string GetFragmentShaderSource() const { return mFragmentShader; }
	std::string GetError() const { return mError; }

	void Bind();
	void Unbind();

	void SetUniformInt(const std::string& name, int val);
	void SetUniformFloat(const std::string& name, float val);
	void SetUniformFloat2(const std::string& name, float val1, float val2);
	void SetUniformFloat2(const std::string& name, const glm::vec2& val);
	void SetUniformFloat3(const std::string& name, float val1, float val2, float val3);
	void SetUniformFloat3(const std::string& name, const glm::vec3& val);
	void SetUniformFloat4(const std::string& name, float val1, float val2, float val3, float val4);
	void SetUniformFloat4(const std::string& name, const glm::vec4& val);
	void SetUniformMat3(const std::string& name, const glm::mat3& mat);
	void SetUniformMat4(const std::string& name, const glm::mat4& mat);

private:
	int GetUniformLocation(const std::string& name);
	bool CheckForErrors(unsigned int shaderID, std::string type);
private:
	uint32_t mProgramId;
	std::string mVertexShader;
	std::string mFragmentShader;
	std::string mError;

	std::unordered_map<std::string, int> mUniformLocations;
};