#include "shader.hpp"
#include "log.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode)
{
	mVertexShader = vertexCode;
	mFragmentShader = fragmentCode;

	mProgramId = glCreateProgram();
	bool status = true;

	uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	if (status)
	{
		const GLchar* vertexSource = mVertexShader.c_str();
		glShaderSource(vertexShaderId, 1, &vertexSource, NULL);
		glCompileShader(vertexShaderId);
		if (!CheckForErrors(vertexShaderId, "vertex"))
		{
			glAttachShader(mProgramId, vertexShaderId);
		}
		else
		{
			status = false;
			LOG("Failed to compile vertex shader");
		}
	}

	uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	if (status)
	{
		const GLchar* fragmentSource = mFragmentShader.c_str();
		glShaderSource(fragmentShaderId, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShaderId);
		if (!CheckForErrors(fragmentShaderId, "fragment"))
		{
			glAttachShader(mProgramId, fragmentShaderId);
		}
		else
		{
			status = false;
			LOG("Failed to compile fragment shader");
		}
	}

	if (status)
	{
		glLinkProgram(mProgramId);
		glValidateProgram(mProgramId);
		if (CheckForErrors(mProgramId, "program"))
		{
			glDeleteProgram(mProgramId);
			mProgramId = -1;
			status = false;
			LOG("Failed to link shader program");
		}
		else
		{
			LOG("Successfully linked shader program");
		}
	}

	if (status)
	{
		LOG("Shader successfully created");
	}
	else
	{
		LOG("Failed to create shader");
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}

Shader::~Shader()
{
	glUseProgram(0);
	glDeleteProgram(mProgramId);
}

void Shader::Bind()
{
	glUseProgram(mProgramId);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniformInt(const std::string& name, int val)
{
	glUseProgram(mProgramId);
	glUniform1i(GetUniformLocation(name), val);
}

void Shader::SetUniformFloat(const std::string& name, float val)
{
	glUseProgram(mProgramId);
	glUniform1f(GetUniformLocation(name), val);
}

void Shader::SetUniformFloat2(const std::string& name, float val1, float val2)
{
	glUseProgram(mProgramId);
	glUniform2f(GetUniformLocation(name), val1, val2);
}

void Shader::SetUniformFloat2(const std::string& name, const glm::vec2& val)
{
	SetUniformFloat2(name, val.x, val.y);
}

void Shader::SetUniformFloat3(const std::string& name, float val1, float val2, float val3)
{
	glUseProgram(mProgramId);
	glUniform3f(GetUniformLocation(name), val1, val2, val3);
}

void Shader::SetUniformFloat3(const std::string& name, const glm::vec3& val)
{
	SetUniformFloat3(name, val.x, val.y, val.z);
}

void Shader::SetUniformFloat4(const std::string& name, float val1, float val2, float val3, float val4)
{
	glUseProgram(mProgramId);
	glUniform4f(GetUniformLocation(name), val1, val2, val3, val4);
}

void Shader::SetUniformFloat4(const std::string& name, const glm::vec4& val)
{
	SetUniformFloat4(name, val.x, val.y, val.z, val.w);
}

void Shader::SetUniformMat3(const std::string& name, const glm::mat3& mat)
{
	glUseProgram(mProgramId);
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat)
{
	glUseProgram(mProgramId);
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

int Shader::GetUniformLocation(const std::string& name)
{
	auto it = mUniformLocations.find(name);
	if (it == mUniformLocations.end())
	{
		mUniformLocations[name] = glGetUniformLocation(mProgramId, name.c_str());
	}

	return mUniformLocations[name];
}

bool Shader::CheckForErrors(unsigned int shaderId, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "program")
	{
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
			LOG("Shader compilation error of type: %s\n%s", type.c_str(), infoLog);
			mError = infoLog;
			return true;
		}
	}
	else
	{
		glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderId, 1024, NULL, infoLog);
			LOG("Program linking error of type: %s\n%s", type.c_str(), infoLog);
			mError = infoLog;
			return true;
		}
	}
	return false;
}