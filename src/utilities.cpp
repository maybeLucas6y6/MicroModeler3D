#include "utilities.hpp"
#include "texture.hpp"
#include "log.hpp"

#include <filesystem>
#include <fstream>

std::map<std::string, std::string> LoadShaders(std::string directory, std::unordered_set<std::string> extensions)
{
	std::filesystem::path path(directory);
	std::map<std::string, std::string> files;
	const std::filesystem::directory_iterator end{};
	for (std::filesystem::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (!std::filesystem::is_regular_file(*iter)) continue;

		auto extension = std::filesystem::path(iter->path()).extension().string();
		if (extensions.contains(extension))
		{
			auto path = iter->path().string();

			std::ifstream file(path);
			std::string code = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string name = path.substr(path.find_last_of('\\') + 1);

			files.insert({ name, code });
		}
	}
	return files;
}

std::map<std::string, std::shared_ptr<Texture>> LoadTextures(std::string directory, std::unordered_set<std::string> extensions)
{
	std::filesystem::path path(directory);
	std::map<std::string, std::shared_ptr<Texture>> files;
	const std::filesystem::directory_iterator end{};
	for (std::filesystem::directory_iterator iter{ path }; iter != end; ++iter)
	{
		if (!std::filesystem::is_regular_file(*iter)) continue;

		auto extension = std::filesystem::path(iter->path()).extension().string();
		if (extensions.contains(extension))
		{
			auto path = iter->path().string();

			std::ifstream file(path);
			std::shared_ptr<Texture> tex = std::make_shared<Texture>(path);
			std::string name = path.substr(path.find_last_of('\\') + 1);

			files.insert({ name, tex });
		}
	}
	return files;
}

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

void CreateFile(const std::string& path, const std::string& name, const char* data)
{
	std::ofstream file(std::string(path + "/" + name));
	file << data;
	LOG("File successfully created: [%s] at path %s", name.data(), path.data());
}

void DeleteFile(const std::string& path, const std::string& name)
{
	if (std::filesystem::remove(std::string(path + "/" + name)))
	{
		LOG("File successfully deleted: [%s] at path %s", name.data(), path.data());
	}
	else
	{
		LOG("Could not delete file [%s] at path %s", name.data(), path.data());
	}
}

void EditFile(const std::string& path, const std::string& oldName, const std::string& newName, const std::string& newContent)
{
	LOG("Old name: %s. New name: %s", oldName.data(), newName.data());
	if (newContent.length() > 0)
	{
		std::ofstream file(path + "/" + oldName);
		file << newContent;
	}
	auto a = path + "/" + oldName;
	auto b = path + "/" + newName;
	std::rename(a.data(), b.data());
}