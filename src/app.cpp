#include "app.hpp"
#include "log.hpp"
#include "utilities.hpp"

#include "framebuffer.hpp"
#include "vertex.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "camera.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_glfw.h"
#include "../external/imgui/imgui_impl_opengl3.h"

Camera* App::mCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool App::mIsUsingCamera = false;
float App::mLastX = 0.0f;
float App::mLastY = 0.0f;
bool App::mFirstMouse = true;
int App::mWindowWidth = 1080;
int App::mWindowHeight = 720;

App::App()
	: mIsRunning(false)
	, mDeltaTime(0.0)
	, mLastFrame(0.0)
	, mCurrentFrame(0.0)
	, mAspectRatio(1.5f)
	, mWindow(nullptr)
	, mView(glm::mat4(0.0f))
	, mProjection(glm::mat4(0.0f))
	, mInSceneView(false)
	, mClearColor(0.3f, 0.3f, 0.3f, 1.0f)
{
	
}

bool App::Initialize()
{
	if (glfwInit() == GLFW_FALSE)
	{
		LOG("Could not initialize GLFW");
		return false;
	}

	mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "MicroModeler3D", nullptr, nullptr);
	if (!mWindow)
	{
		LOG("Could not create GLFW window");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwMakeContextCurrent(mWindow);
	glfwSetCursorPosCallback(mWindow, MouseCallback);
	glfwSetScrollCallback(mWindow, ScrollCallback);
	glfwSetFramebufferSizeCallback(mWindow, FramebufferResizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG("Could not load GLAD");
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::PushStyleColor(ImGuiCol_Header, { 0.2f, 0.2f, 0.2f, 0.2f });

	LoadAssets();

	glViewport(0, -180, mWindowWidth, mWindowWidth);

	mIsRunning = true;
	return true;
}

void App::Update()
{
	mCurrentFrame = glfwGetTime();
	mDeltaTime = mCurrentFrame - mLastFrame;
	mLastFrame = mCurrentFrame;

	mView = mCamera->GetViewMatrix();

	mProjection = glm::perspective(glm::radians(mCamera->GetZoom()), static_cast<float>(mWindowWidth / mWindowHeight), 0.1f, 100.0f);

	for (auto& e : mEntities)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, e.second->translate);
		model = glm::rotate(model, glm::radians(e.second->angle), e.second->rotate);
		model = glm::scale(model, e.second->scale);
		e.second->model = model;
	}

	ProcessInput();

	if (glfwWindowShouldClose(mWindow))
	{
		mIsRunning = false;
	}

	glfwPollEvents();
}

void App::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer->GetFB());
	glViewport(0, 0, mFramebuffer->GetSize().x, mFramebuffer->GetSize().y);
	glEnable(GL_DEPTH_TEST);
	glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& e : mEntities)
	{
		RenderEntity(e.second);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, -180, mWindowWidth, mWindowWidth);

	if (mInSceneView)
	{
		glDisable(GL_DEPTH_TEST);

		mFramebufferRect->Bind();
		glBindTexture(GL_TEXTURE_2D, mFramebuffer->GetTextureId());
		mFramebufferShader->Bind();

		glDrawElements(GL_TRIANGLES, mFramebufferRect->GetElementCount(), GL_UNSIGNED_INT, 0);

		mFramebufferShader->Unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
		mFramebufferRect->Unbind();
	}
	else 
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		}

		ImGuiRender();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	glfwSwapBuffers(mWindow);
}

void App::Shutdown()
{
	delete mCamera;
	mIsRunning = false;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void App::LoadAssets()
{
	mFramebuffer = std::make_shared<Framebuffer>(mWindowWidth, mWindowHeight);

	mFramebufferRect = std::make_shared<VertexArray>();
	{
		std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>();
		vb->PushVertex({ 1.0f,  1.0f,		1.0f, 1.0f });
		vb->PushVertex({ 1.0f, -1.0f,		1.0f, 0.0f });
		vb->PushVertex({ -1.0f, -1.0f, 		0.0f, 0.0f });
		vb->PushVertex({ -1.0f,  1.0f,		0.0f, 1.0f });
		vb->SetLayout({ 2, 2 });
		mFramebufferRect->PushBuffer(std::move(vb));
	}
	mFramebufferRect->SetElements({ 0, 3, 1, 1, 3, 2 });
	mFramebufferRect->Upload();

	std::shared_ptr<VertexArray> cubeVA = std::make_shared<VertexArray>();
	{

		std::unique_ptr<VertexBuffer> vb = std::make_unique<VertexBuffer>();
		vb->PushVertex({-0.5f, -0.5f, -0.5f, 0.0f, 0.0f });
		vb->PushVertex({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f });
		vb->PushVertex({ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f });
		vb->PushVertex({-0.5f, 0.5f, -0.5f, 0.0f, 1.0f });

		vb->PushVertex({-0.5f, -0.5f, 0.5f, 0.0f, 0.0f });
		vb->PushVertex({ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f });
		vb->PushVertex({ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f });
		vb->PushVertex({-0.5f, 0.5f, 0.5f, 0.0f, 1.0f });

		vb->PushVertex({-0.5f, 0.5f, 0.5f, 1.0f, 0.0f });
		vb->PushVertex({-0.5f, 0.5f, -0.5f, 1.0f, 1.0f });
		vb->PushVertex({-0.5f, -0.5f, -0.5f, 0.0f, 1.0f });
		vb->PushVertex({-0.5f, -0.5f, 0.5f, 0.0f, 0.0f });

		vb->PushVertex({ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f });
		vb->PushVertex({ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f });
		vb->PushVertex({ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f });
		vb->PushVertex({ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f });

		vb->PushVertex({-0.5f, -0.5f, -0.5f, 0.0f, 1.0f });
		vb->PushVertex({ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f });
		vb->PushVertex({ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f });
		vb->PushVertex({-0.5f, -0.5f, 0.5f, 0.0f, 0.0f });

		vb->PushVertex({-0.5f, 0.5f, -0.5f, 0.0f, 1.0f });
		vb->PushVertex({ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f });
		vb->PushVertex({ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f });
		vb->PushVertex({-0.5f, 0.5f, 0.5f, 0.0f, 0.0f });

		vb->SetLayout({ 3, 2 });

		cubeVA->PushBuffer(std::move(vb));
	}
	cubeVA->SetElements({
		2, 1, 0, 2, 0, 3,
		6, 5, 4, 6, 4, 7,
		10, 9, 8, 10, 8, 11,
		14, 13, 12, 14, 12, 15,
		18, 17, 16, 18, 16, 19,
		22, 21, 20, 22, 20, 23
	});
	cubeVA->Upload();

	mVAs.insert({ "cube", cubeVA });

	mTextures = LoadTextures("resources/textures", { ".png", ".jpg" });
	mShaders = LoadShaders("resources/shaders", { ".vert", ".frag" });

	mFramebufferShader = std::make_shared<Shader>(mShaders["framebuffer.vert"], mShaders["framebuffer.frag"]);

	mTextData = std::shared_ptr<InputTextCallback_UserData>();
}

void App::ProcessInput()
{
	if (mIsUsingCamera)
	{
		if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::FORWARD, static_cast<float>(mDeltaTime));
		if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::BACKWARD, static_cast<float>(mDeltaTime));
		if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::LEFT, static_cast<float>(mDeltaTime));
		if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::RIGHT, static_cast<float>(mDeltaTime));
		if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::UP, static_cast<float>(mDeltaTime));
		if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			mCamera->ProcessKeyboard(CameraMovement::DOWN, static_cast<float>(mDeltaTime));
	}
	if (mInSceneView || mIsUsingCamera)
	{
		if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mIsUsingCamera = false;
			mInSceneView = false;
		}
	}
}

void App::MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (mFirstMouse)
	{
		mLastX = xpos;
		mLastY = ypos;
		mFirstMouse = false;
	}

	float xoffset = xpos - mLastX;
	float yoffset = mLastY - ypos;

	mLastX = xpos;
	mLastY = ypos;

	if (mIsUsingCamera)
	{
		mCamera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void App::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (mIsUsingCamera)
	{
		mCamera->ProcessMouseScroll(static_cast<float>(yoffset));
	}
}

void App::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	mWindowWidth = width;
	mWindowHeight = height;
	glViewport(0, 0, mWindowWidth, mWindowHeight);
}

void App::ImGuiRender()
{
	static std::string selectedEntity = "##";
	static std::string selectedShader = "##";
	static bool changedShader = false;
	static std::string selectedMaterial = "##";

	if (ImGui::Begin("Settings"))
	{
		auto camPos = mCamera->GetPosition();
		ImGui::Text("Camera position x:%.2f, y:%.2f, z:%.2f", camPos.x, camPos.y, camPos.z);

		if (ImGui::Button("Go Fullscreen"))
		{
			mInSceneView = true;
			mIsUsingCamera = true;
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		ImGui::PushItemWidth(200.0f);
		ImGui::ColorPicker4("Background Color", &mClearColor.r, ImGuiColorEditFlags_DefaultOptions2_);
	}
	ImGui::End();

	if (ImGui::Begin("Entities"))
	{
		for (auto& entry : mEntities)
		{
			if (ImGui::Selectable(std::string(entry.first + "##").c_str(), entry.first == selectedEntity))
			{
				if (entry.first == selectedEntity)
				{
					selectedEntity = "##";
				}
				else
				{
					selectedEntity = entry.first;
					ImGui::SetNextWindowFocus();
				}
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Edit entity"))
	{
		if (selectedEntity != "##")
		{
			auto& e = mEntities[selectedEntity];

			ImGui::Text(std::string("Entity name: " + selectedEntity).c_str());

			ImGui::SeparatorText("Position");
			ImGui::DragFloat3("##Position", &e->translate[0], 0.1f, 0.0f, 0.0f, "%.2f");

			ImGui::SeparatorText("Rotation");
			bool x = e->rotate[0], y = e->rotate[1], z = e->rotate[2];
			ImGui::Text("Rotate on the");
			ImGui::SameLine();
			if (ImGui::Checkbox("X", &x))
			{
				e->rotate[0] = x;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Y", &y))
			{
				e->rotate[1] = y;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Z", &z))
			{
				e->rotate[2] = z;
			}
			ImGui::SameLine();
			ImGui::Text(" axis");
			ImGui::DragFloat("##Angle", &e->angle, 1.0f, 0.0f, 0.0f, "%.2f");

			ImGui::SeparatorText("Scale");
			ImGui::DragFloat3("##Scale", &e->scale[0], 0.1f, 0.0f, 0.0f, "%.2f");

			if (ImGui::Button("Destroy"))
			{
				mEntities.erase(selectedEntity);
				selectedEntity = "##";
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Create shader", nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		static std::string code;
		auto size = ImGui::GetWindowSize();
		InputTextMultiline("##Code editor", &code, { size.x - 50.0f, size.y - 100.0f }, ImGuiInputTextFlags_AllowTabInput, InputTextCallback, &mTextData);

		static char name[21] = { 0 };
		ImGui::InputText("Name", name, 20, ImGuiInputTextFlags_AutoSelectAll);

		static int current = 0;
		static const std::vector<const char*> options{ "Vertex Shader", "Fragment Shader" };
		ImGui::Combo("Type", &current, options.data(), (int)options.size(), 2);

		if (ImGui::Button("Upload") && strlen(name) && !mShaders.contains(name))
		{
			std::string extension = current == 0 ? ".vert" : ".frag";
			auto nameExt = name + extension;

			mShaders.insert({ nameExt, code });
			CreateFile("resources/shaders", nameExt, code.data());

			memset(name, 0, 21);
			code.clear();
		}
	}
	ImGui::End();

	if (ImGui::Begin("Shaders"))
	{
		for (auto& s : mShaders)
		{
			if (ImGui::Selectable(s.first.data(), s.first == selectedShader))
			{
				if (s.first == selectedShader)
				{
					selectedShader = "##";
				}
				else
				{
					selectedShader = s.first;
					ImGui::SetNextWindowFocus();
				}
				changedShader = true;
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Edit shader", nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		static bool editingShader = false;

		if (selectedShader != "##")
		{
			ImGui::Text("Shader: %s", selectedShader.data());

			static std::string code;
			static std::string name;
			static char newName[21] = { 0 };
			static int current = -1;
			static auto flags = ImGuiInputTextFlags_ReadOnly;

			if (changedShader)
			{
				code = mShaders[selectedShader];

				auto dotPos = selectedShader.find_first_of('.');
				name = selectedShader.substr(0, dotPos);

				auto type = selectedShader.substr(dotPos + 1);
				current = type == "vert" ? 0 : 1;

				changedShader = false;

				memset(newName, 0, 21);

				flags = ImGuiInputTextFlags_ReadOnly;
				editingShader = false;
			}

			auto size = ImGui::GetWindowSize();
			InputTextMultiline("##Shader editor", &code, { size.x - 50.0f, size.y - 120.0f }, flags, InputTextCallback, &mEditShaderTextData);

			if (editingShader)
			{
				ImGui::InputText("New Name", newName, 20, ImGuiInputTextFlags_AutoSelectAll);

				static const std::vector<const char*> options{ "Vertex Shader", "Fragment Shader" };
				ImGui::Combo("New Type", &current, options.data(), (int)options.size(), 2);

				if (ImGui::Button("Save") && newName[0] && (!mShaders.contains(newName) || newName == selectedShader))
				{
					mShaders.erase(selectedShader);
					std::string extension = current == 0 ? ".vert" : ".frag";
					auto nameExt = std::string(newName) + extension;
					mShaders.insert({ nameExt, code });

					LOG(nameExt.data());
					EditFile("resources/shaders", selectedShader, nameExt, code);

					selectedShader = nameExt;
					changedShader = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					changedShader = true;
				}
			}
			else {
				if (ImGui::Button("Edit"))
				{
					editingShader = true;
					flags = ImGuiInputTextFlags_AllowTabInput;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				DeleteFile("resources/shaders", selectedShader);
				mShaders.erase(selectedShader);
				selectedShader = "##";
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Create material"))
	{
		static int currentVertexShader = -1;
		std::vector<const char*> vertexShaderOptions;
		for (auto& shader : mShaders)
		{
			auto extension = shader.first.substr(shader.first.find_last_of('.'));
			if (extension != ".vert") continue;
			vertexShaderOptions.push_back(shader.first.c_str());
		}
		ImGui::Combo("Vertex shader", &currentVertexShader, vertexShaderOptions.data(), (int)vertexShaderOptions.size(), 4);

		static int currentFragmentShader = -1;
		std::vector<const char*> fragmentShaderOptions;
		for (auto& shader : mShaders)
		{
			auto extension = shader.first.substr(shader.first.find_last_of('.'));
			if (extension != ".frag") continue;
			fragmentShaderOptions.push_back(shader.first.c_str());
		}
		ImGui::Combo("Fragment shader", &currentFragmentShader, fragmentShaderOptions.data(), (int)fragmentShaderOptions.size(), 4);

		static int currentTexture = 0;
		std::vector<const char*> textureOptions;
		textureOptions.push_back("None");
		for (auto& tex : mTextures)
		{
			textureOptions.push_back(tex.first.c_str());
		}
		ImGui::Combo("Texture", &currentTexture, textureOptions.data(), (int)textureOptions.size(), 4);

		static char name[21] = { 0 };
		ImGui::InputText("Name", name, 20, ImGuiInputTextFlags_AutoSelectAll);

		static bool compileError = false;
		static std::string errorMsg;

		if (ImGui::Button("Create"))
		{
			if (currentVertexShader != -1 && currentFragmentShader != -1 && strlen(name) && !mMaterials.contains(name))
			{
				std::string vertexCode(mShaders[vertexShaderOptions[currentVertexShader]]);
				std::string fragmentCode(mShaders[fragmentShaderOptions[currentFragmentShader]]);
				auto shader = std::make_shared<Shader>(vertexCode, fragmentCode);
				errorMsg = shader->GetError();

				if (errorMsg.length() == 0)
				{
					auto mat = std::make_shared<Material>(shader, currentTexture == 0 ? nullptr : mTextures[textureOptions[currentTexture]]);
					mMaterials.insert({name, mat});
					LOG("Material successfully created: %s", name);

					memset(name, 0, 21);
					currentVertexShader = -1;
					currentFragmentShader = -1;
					currentTexture = 0;
				}
				else
				{
					compileError = true;

					LOG("Material %s not created", name);
				}
			}
		}

		if (compileError) ImGui::OpenPopup("Material creation failed");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Material creation failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Failed to create material. Shader compilation failed");
			ImGui::Text("Error message: \n%s", errorMsg.data());

			if (ImGui::Button("Close"))
			{
				compileError = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();

	if (ImGui::Begin("Create object"))
	{
		static int currentVA = -1;
		std::vector<const char*> vaOptions;
		for (auto& va : mVAs)
		{
			vaOptions.push_back(va.first.c_str());
		}
		ImGui::Combo("Vertex Array", &currentVA, vaOptions.data(), (int)vaOptions.size(), 4);

		static int currentMaterial = -1;
		std::vector<const char*> materialOptions;
		for (auto& mat : mMaterials)
		{
			materialOptions.push_back(mat.first.c_str());
		}
		ImGui::Combo("Material", &currentMaterial, materialOptions.data(), (int)materialOptions.size(), 4);

		static char name[21] = { 0 };
		ImGui::InputText("Name", name, 20, ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::Button("Create"))
		{
			if (strlen(name) && !mObjects.contains(name) && currentVA != -1 && currentMaterial != -1)
			{
				auto object = std::make_shared<Object>();
				object->vaName = vaOptions[currentVA];
				object->matName = materialOptions[currentMaterial];
				object->va = mVAs[vaOptions[currentVA]];
				object->mat = mMaterials[materialOptions[currentMaterial]];
				mObjects.insert({ name, object });

				memset(name, 0, 21);
				currentVA = -1;
				currentMaterial = -1;
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Create entity"))
	{
		static int current = 0;
		std::vector<const char*> options;
		for (auto& asset : mObjects)
		{
			options.push_back(asset.first.c_str());
		}
		ImGui::Combo("Object", &current, options.data(), (int)options.size(), 4);

		static glm::vec3 translate = glm::vec3(0.0f);
		ImGui::InputFloat3("Translate", &translate[0]);

		static char name[21] = { 0 };
		ImGui::InputText("Name", name, 20, ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::Button("Create"))
		{
			if (strlen(name))
			{
				if (!mEntities.contains(name))
				{
					std::shared_ptr<Entity> e = std::make_shared<Entity>();
					e->object = mObjects[options[current]];
					e->translate = translate;
					mEntities.insert({ name, e });
					LOG("Entity successfully created: %s", name);

					memset(name, 0, 21);
					translate = glm::vec3(0.0f);
				}
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Objects"))
	{
		for (auto& entry : mObjects)
		{
			if (ImGui::CollapsingHeader(entry.first.c_str()))
			{
				auto& mat = entry.second->matName;
				ImGui::Text("Material: ");
				ImGui::SameLine();
				if (ImGui::Button(mat.data()))
				{
					selectedMaterial = selectedMaterial == mat ? "##" : mat;
					if (selectedMaterial != "##")
					{
						ImGui::SetNextWindowFocus();
					}
				}

				auto& va = entry.second->vaName;
				ImGui::Text("VA: ");
				ImGui::SameLine();
				if (ImGui::Button(va.data()))
				{
					// for future development
				}

				if (ImGui::Button("Delete"))
				{
					mObjects.erase(entry.first);
					break;
				}
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Materials"))
	{
		for (auto& entry : mMaterials)
		{
			auto& mat = entry.second;
			if (selectedMaterial == entry.first)
			{
				ImGui::SetNextItemOpen(true);
				selectedMaterial = "##";
			}
			if (ImGui::CollapsingHeader(entry.first.c_str()))
			{
				ImGui::SeparatorText("Vertex shader");
				auto vertex = mat->GetShader()->GetVertexShaderSource();
				ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, vertex.data());

				ImGui::SeparatorText("Fragment shader");
				auto fragment = mat->GetShader()->GetFragmentShaderSource();
				ImGui::TextColored({ 0.7f, 0.7f, 0.7f, 1.0f }, fragment.data());

				auto tex = mat->GetTexture();
				if (tex)
				{
					ImGui::SeparatorText("Texture");
					ImGui::Image((void*)(intptr_t)tex->GetId(), { 300.0f, 300.0f }, {0.0f, 1.0f}, {1.0f, 0.0f});
				}

				if (ImGui::Button("Delete"))
				{
					mMaterials.erase(entry.first);
					break;
				}
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Textures"))
	{
		for (auto& t : mTextures)
		{
			if (ImGui::Selectable(std::string(t.first + "##").c_str()))
			{

			}
			if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
			{
				auto& tex = t.second;
				ImGui::Image((void*)(intptr_t)tex->GetId(), { (float)tex->GetWidth(), (float)tex->GetHeight() }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
				ImGui::EndTooltip();
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("SceneView"))
	{
		if (ImGui::IsWindowHovered())
		{
			ImGui::CaptureMouseFromApp(false);
		}

		ImVec2 winsize = ImGui::GetWindowSize();
		winsize.x -= 15, winsize.y -= 35;
		ImVec2 size{ winsize.y * mAspectRatio, winsize.x / mAspectRatio }; // TODO: figure out this
		if (size.x >= winsize.x)
			size.x = winsize.x;
		else
			size.y = winsize.y;
		ImVec2 pos{ (winsize.x - size.x) * 0.5f, ((winsize.y - size.y) * 0.5f) + 10 };
		ImGui::SetCursorPos(pos);
		static constexpr ImVec2 uv0 = { 0.0f, -180.0f / 1080.0f };
		static constexpr ImVec2 uv1 = { 1.0f, -(180.0f + 720.0f) / 1080.f };
		ImGui::Image((void*)(intptr_t)mFramebuffer->GetTextureId(), size, uv0, uv1);

		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				mIsUsingCamera = true;
				glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
			{
				mIsUsingCamera = false;
				glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}
	ImGui::End();
}

void App::RenderEntity(std::shared_ptr<Entity> entity)
{
	auto& va = entity->object->va;
	auto shader = entity->object->mat->GetShader();
	auto tex = entity->object->mat->GetTexture();

	va->Bind();
	shader->Bind();
	if (tex)
	{
		tex->Bind();
	}

	entity->object->mat->UpdateShaderUniforms();

	shader->SetUniformMat4("model", entity->model);
	shader->SetUniformMat4("proj", mProjection);
	shader->SetUniformMat4("view", mView);

	if (va->GetElementCount() > 0)
	{
		glDrawElements(GL_TRIANGLES, va->GetElementCount(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLE_STRIP, 0, va->GetVertexCount());
	}

	if (tex)
	{
		tex->Unbind();
	}
	shader->Unbind();
	va->Unbind();
}