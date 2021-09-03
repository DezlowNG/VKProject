#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Core/Vulkan/VkDeleter.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <Components/Camera/Camera.h>

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std 
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((std::hash<glm::vec3>()(vertex.pos) ^ (std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (std::hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float deltaTime;
};

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);

class Application
{
public:
	void Run()
	{
		initWindow();
		initVulkan();
		mainLoop();
	}
private:
	GLFWwindow* mWindow = nullptr;

	double currentFrame = 0.0f;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = 0.0f;
	float lastY = 0.0f;

	uint32_t mMipLevels = 0;

	VkDeleter<VkInstance> mInstance{ vkDestroyInstance };
	VkDeleter<VkDebugReportCallbackEXT> mCallback{ mInstance, DestroyDebugReportCallbackEXT };
	VkDeleter<VkSurfaceKHR> mSurface{ mInstance, vkDestroySurfaceKHR };
	VkDeleter<VkDevice> mDevice{ vkDestroyDevice };
	VkDeleter<VkSwapchainKHR> mSwapChain{ mDevice, vkDestroySwapchainKHR };
	VkDeleter<VkRenderPass> mRenderPass{ mDevice, vkDestroyRenderPass };
	VkDeleter<VkDescriptorSetLayout> mDescriptorSetLayout{ mDevice, vkDestroyDescriptorSetLayout };
	VkDeleter<VkPipelineLayout> mPipelineLayout{ mDevice, vkDestroyPipelineLayout };
	VkDeleter<VkPipeline> mGraphicsPipeline{ mDevice, vkDestroyPipeline };
	VkDeleter<VkCommandPool> mCommandPool{ mDevice, vkDestroyCommandPool };

	VkDeleter<VkImage> mColorImage{ mDevice, vkDestroyImage };
	VkDeleter<VkDeviceMemory> mColorImageMemory{ mDevice, vkFreeMemory };
	VkDeleter<VkImageView> mColorImageView{ mDevice, vkDestroyImageView };

	VkDeleter<VkImage> mDepthImage{ mDevice, vkDestroyImage };
	VkDeleter<VkDeviceMemory> mDepthImageMemory{ mDevice, vkFreeMemory };
	VkDeleter<VkImageView> mDepthImageView{ mDevice, vkDestroyImageView };

	VkDeleter<VkImage> mTextureImage{ mDevice, vkDestroyImage };
	VkDeleter<VkDeviceMemory> mTextureImageMemory{ mDevice, vkFreeMemory };
	VkDeleter<VkImageView> mTextureImageView{ mDevice, vkDestroyImageView };
	VkDeleter<VkSampler> mTextureSampler{ mDevice, vkDestroySampler };

	VkDeleter<VkBuffer> mVertexBuffer{ mDevice, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> mVertexBufferMemory{ mDevice, vkFreeMemory };
	
	VkDeleter<VkBuffer> mIndexBuffer{ mDevice, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> mIndexBufferMemory{ mDevice, vkFreeMemory };

	VkDeleter<VkBuffer> mUniformBuffer{ mDevice, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> mUniformBufferMemory{ mDevice, vkFreeMemory };

	VkDeleter<VkDescriptorPool> mDescriptorPool{ mDevice, vkDestroyDescriptorPool };
	VkDescriptorSet mDescriptorSet;

	std::vector<VkCommandBuffer> mCommandBuffers;

	VkDeleter<VkSemaphore> mImageAvailableSemaphore{ mDevice, vkDestroySemaphore };
	VkDeleter<VkSemaphore> mRenderFinishedSemaphore{ mDevice, vkDestroySemaphore };

	std::vector<VkDeleter<VkImageView>> mSwapChainImageViews;
	std::vector<VkDeleter<VkFramebuffer>> mSwapChainFramebuffers;
	
	VkPhysicalDevice mPhysDevice = VK_NULL_HANDLE;
	VkQueue mGraphicsQueue;
	VkQueue mPresentQueue;
	
	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;

	std::vector<VkImage> mSwapChainImages;

	const unsigned int mWIDTH = 1600;
	const unsigned int mHEIGHT = 900;

	Camera mCamera;
	bool mCameraInput = true;

	const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor" };
	const std::vector<const char*> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;

	const std::string MODEL_PATH = "Models/viking_room.obj";
	const std::string TEXTURE_PATH = "Textures/viking_room.png";

	VkSampleCountFlagBits mMSAASamples = VK_SAMPLE_COUNT_1_BIT;

#ifdef NDEBUG
	const bool mEnableValidationLayers = false;
#else
	const bool mEnableValidationLayers = true;
#endif

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);

	static std::vector<char> readFile(const std::string& fileName);
	
	static void onWindowResized(GLFWwindow* window, int width, int height);
	static void onWindowCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	void calculateDelta();

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();

	bool checkValidationLayerSupport();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
	bool hasStencilComponent(VkFormat format);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& canditates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	VkSampleCountFlagBits getMaxUsableSampleCount();

	void createShaderModule(const std::vector<char>& code, VkDeleter<VkShaderModule>& shaderModule);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props, VkDeleter<VkImage>& image, VkDeleter<VkDeviceMemory>& imageMemory, uint32_t mipLevels, VkSampleCountFlagBits numSamples);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevel);
	void copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDeleter<VkImageView>& imageView, uint32_t mipLevels);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void initWindow();

	void setupDebugCallback();
	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createColorResources();
	void createDepthResources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void loadModel();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffer();
	void createDescriptorPool();
	void createDescriptorSet();
	void createCommandBuffers();
	void createSemaphores();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkDeleter<VkBuffer>& buffer, VkDeleter<VkDeviceMemory>& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void recreateSwapChain();
	void updateUniformBuffer();

	void initVulkan();

	void drawScene();

	void mainLoop();
};

#endif