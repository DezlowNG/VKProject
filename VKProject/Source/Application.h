#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeleter.h>

#include <glm/glm.hpp>

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
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
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

	const unsigned int mWIDTH = 800;
	const unsigned int mHEIGHT = 600;

	const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor" };
	const std::vector<const char*> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<Vertex> mVertices = {
		{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
	};

	const std::vector<uint16_t> mIndices = {
		0, 1, 2, 2, 3, 0
	};

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

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();

	bool checkValidationLayerSupport();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createShaderModule(const std::vector<char>& code, VkDeleter<VkShaderModule>& shaderModule);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);

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