#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include <string>

struct GLFWwindow;
struct InputTextCallback_UserData;

class Framebuffer;

class VertexArray;
class Material;
struct Object
{
	std::string vaName, matName;
	std::shared_ptr<VertexArray> va;
	std::shared_ptr<Material> mat;
};
struct Entity
{
	std::shared_ptr<Object> object;
	glm::vec3 translate = glm::vec3(0.0f);
	float angle = 0.0f;
	glm::vec3 rotate = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
};

class Shader;
class Texture;

class Camera;

class App
{
public:
	App();

	bool Initialize();
	void Update();
	void Render();
	void Shutdown();

	bool IsRunning() const { return mIsRunning; }
private:
	bool mIsRunning;

	double mDeltaTime, mLastFrame, mCurrentFrame;
	
	static int mWindowWidth, mWindowHeight;
	float mAspectRatio;
	GLFWwindow* mWindow;
	std::shared_ptr<Framebuffer> mFramebuffer;
	std::shared_ptr<VertexArray> mFramebufferRect;
	std::shared_ptr<Shader> mFramebufferShader;

	std::map<std::string, std::shared_ptr<Texture>> mTextures;
	std::map<std::string, std::string> mShaders;
	std::map<std::string, std::shared_ptr<Material>> mMaterials;
	std::map<std::string, std::shared_ptr<VertexArray>> mVAs;
	std::map<std::string, std::shared_ptr<Object>> mObjects;
	std::unordered_map<std::string, std::shared_ptr<Entity>> mEntities;

	static Camera* mCamera;
	static bool mIsUsingCamera;
	static float mLastX, mLastY;
	static bool mFirstMouse;
	glm::mat4 mView, mProjection;
	glm::vec4 mClearColor;

	bool mInSceneView;

	std::shared_ptr<InputTextCallback_UserData> mTextData;
	std::shared_ptr<InputTextCallback_UserData> mEditShaderTextData;

	void LoadAssets();
	void ProcessInput();
	static void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	void ImGuiRender();
	void RenderEntity(std::shared_ptr<Entity> entity);
};