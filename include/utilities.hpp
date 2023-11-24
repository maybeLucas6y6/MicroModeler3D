#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include <memory>

#include "../external/imgui/imgui.h"

class Texture;
class Shader;

std::map<std::string, std::string> LoadShaders(std::string directory, std::unordered_set<std::string> extensions);
std::map<std::string, std::shared_ptr<Texture>> LoadTextures(std::string directory, std::unordered_set<std::string> extensions);

struct InputTextCallback_UserData;

int InputTextCallback(ImGuiInputTextCallbackData* data);

bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);

void CreateFile(const std::string& path, const std::string& name, const char* data);
void DeleteFile(const std::string& path, const std::string& name);
void EditFile(const std::string& path, const std::string& oldName, const std::string& newName, const std::string& newContent = "");