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

	VkDeleter<VkInstance> instance{ vkDestroyInstance };
	VkDeleter<VkDebugReportCallbackEXT> callback{ instance, DestroyDebugReportCallbackEXT };
	VkDeleter<VkSurfaceKHR> surface{ instance, vkDestroySurfaceKHR };
	VkDeleter<VkDevice> device{ vkDestroyDevice };
	VkDeleter<VkSwapchainKHR> swapChain{ device, vkDestroySwapchainKHR };
	VkDeleter<VkRenderPass> renderPass{ device, vkDestroyRenderPass };
	VkDeleter<VkPipelineLayout> pipelineLayout{ device, vkDestroyPipelineLayout };
	VkDeleter<VkPipeline> graphicsPipeline{ device, vkDestroyPipeline };
	VkDeleter<VkCommandPool> commandPool{ device, vkDestroyCommandPool };
	VkDeleter<VkBuffer> vertexBuffer{ device, vkDestroyBuffer };
	VkDeleter<VkDeviceMemory> vertexBufferMemory{ device, vkFreeMemory };

	VkDeleter<VkSemaphore> imageAvailableSemaphore{ device, vkDestroySemaphore };
	VkDeleter<VkSemaphore> renderFinishedSemaphore{ device, vkDestroySemaphore };

	std::vector<VkDeleter<VkImageView>> swapChainImageViews;
	std::vector<VkDeleter<VkFramebuffer>> swapChainFramebuffers;
	
	VkPhysicalDevice physDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue, presentQueue;
	
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkCommandBuffer> commandBuffers;

	const unsigned int mWIDTH = 800;
	const unsigned int mHEIGHT = 600;

	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
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

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	std::vector<const char*> getRequiredExtensions();

	bool checkValidationLayerSupport();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionsSupport(VkPhysicalDevice device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);

	void createShaderModule(const std::vector<char>& code, VkDeleter<VkShaderModule>& shaderModule);

	void initWindow();

	void setupDebugCallback();
	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createVertexBuffer();
	void createCommandBuffers();
	void createSemaphores();

	void recreateSwapChain();

	static void onWindowResized(GLFWwindow* window, int width, int height);

	void initVulkan();

	void drawScene();

	void mainLoop();
};

#endif