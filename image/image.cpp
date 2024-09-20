#include <azure_ai_contentsafety_image.h>
#include <iostream>
#include <chrono>
#include "thread"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <conio.h>

using namespace Azure::AI::ContentSafety;

const static std::string encodeLookup("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
const static char padCharacter = '=';

std::map<std::string, std::string> readConfig(const std::string& filename) {
	std::map<std::string, std::string> config;
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file");
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '=')) {
			std::string value;
			if (std::getline(is_line, value)) {
				config[key] = value;
			}
		}
	}
	return config;
}

std::string getCategoryName(ImageCategory category) {
	switch (category) {
	case ImageCategory::Hate:
		return "Hate";
	case ImageCategory::SelfHarm:
		return "Self Harm";
	case ImageCategory::Sexual:
		return "Sexual";
	case ImageCategory::Violence:
		return "Violence";
	default:
		return "Unknown";
	}
}

std::string base64_encode(const std::vector<uint8_t>& input) {
	std::string encoded;
	encoded.reserve(((input.size() / 3) + (input.size() % 3 > 0)) * 4);
	uint32_t temp;
	std::vector<uint8_t>::const_iterator cursor = input.begin();
	for (size_t idx = 0; idx < input.size() / 3; idx++) {
		temp = (*cursor++) << 16;  //Convert to big endian
		temp += (*cursor++) << 8;
		temp += (*cursor++);
		encoded.append(1, encodeLookup[(temp & 0x00FC0000) >> 18]);
		encoded.append(1, encodeLookup[(temp & 0x0003F000) >> 12]);
		encoded.append(1, encodeLookup[(temp & 0x00000FC0) >> 6]);
		encoded.append(1, encodeLookup[(temp & 0x0000003F)]);
	}
	switch (input.size() % 3) {
	case 1:
		temp = (*cursor++) << 16;
		encoded.append(1, encodeLookup[(temp & 0x00FC0000) >> 18]);
		encoded.append(1, encodeLookup[(temp & 0x0003F000) >> 12]);
		encoded.append(2, padCharacter);
		break;
	case 2:
		temp = (*cursor++) << 16;
		temp += (*cursor++) << 8;
		encoded.append(1, encodeLookup[(temp & 0x00FC0000) >> 18]);
		encoded.append(1, encodeLookup[(temp & 0x0003F000) >> 12]);
		encoded.append(1, encodeLookup[(temp & 0x00000FC0) >> 6]);
		encoded.append(1, padCharacter);
		break;
	}
	encoded = encoded;
	return encoded;
}

std::string fileToBase64(const std::string& filePath) {
	// Open the file in binary mode
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + filePath);
	}

	// Read the file into a byte vector
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

	// Encode the byte vector to base64
	std::string base64String = base64_encode(buffer);

	return base64String;
}

int main() {
	std::map<std::string, std::string> config = readConfig("config.ini");
	std::string licenseText = config["licenseText"];

	ImageModelConfig aacsConfig;
	aacsConfig.gpuEnabled = (config["gpuEnabled"] == "true");
	aacsConfig.gpuDeviceId = std::stoi(config["gpuDeviceId"]);
	aacsConfig.numThreads = std::stoi(config["numThreads"]);
	aacsConfig.modelDirectory = config["modelDirectory"];
	aacsConfig.modelName = config["modelName"];

	std::cout << "gpuEnabled: " << aacsConfig.gpuEnabled << std::endl;
	std::cout << "gpuDeviceId: " << aacsConfig.gpuDeviceId << std::endl;
	std::cout << "numThreads: " << aacsConfig.numThreads << std::endl;
	std::cout << "modelDirectory: " << aacsConfig.modelDirectory << std::endl;
	std::cout << "modelName: " << aacsConfig.modelName << std::endl;
	// Get the starting timepoint
	auto start = std::chrono::high_resolution_clock::now();
	ImageModelRuntime aacs(licenseText.c_str(), aacsConfig);
	// Get the ending timepoint
	auto end = std::chrono::high_resolution_clock::now();
	auto modelLoadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Model loaded successfully, duration: " << modelLoadDuration.count() << " milliseconds" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------" << std::endl;
	start = std::chrono::high_resolution_clock::now();
	aacs.Unload();
	end = std::chrono::high_resolution_clock::now();
	auto modelOffloadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Model offloaded successfully, duration: " << modelOffloadDuration.count() << " milliseconds"
		<< std::endl;
	std::cout << "--------------------------------------------------------------------------------------" << std::endl;
	start = std::chrono::high_resolution_clock::now();
	aacs.Reload();
	end = std::chrono::high_resolution_clock::now();
	auto modelReloadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Model Reloaded successfully, duration: " << modelReloadDuration.count() << " milliseconds"
		<< std::endl;
	std::cout << "--------------------------------------------------------------------------------------" << std::endl;

	auto startTime = std::chrono::high_resolution_clock::now();

	std::string folderPath = config["inputImagesDirectory"];

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			std::string filePath = entry.path().string();
			try {
				std::string base64String = fileToBase64(filePath);
				std::cout << "File: " << filePath << std::endl;
				//std::cout << "Base64: " << base64String << std::endl << std::endl;
				AnalyzeImageOptions request;
				request.imageData = base64String; // base64 encoded image data
				std::cout << "  Analyze Image Result: " << std::endl;
				auto severityThreshold = 3;
				// Run inference
				auto analyzeStart = std::chrono::high_resolution_clock::now();
				auto* result = new AnalyzeImageResult();
				aacs.AnalyzeImage(request, *result);
				// Print the result to the console
				for (const auto& categoryAnalysis : result->categoriesAnalysis) {
					if (categoryAnalysis.severity > 0 && categoryAnalysis.severity < severityThreshold) {
						std::cout << "\033[33m";  // Set the text color to yellow
					}
					else if (categoryAnalysis.severity >= severityThreshold) {
						std::cout << "\033[31m";  // Set the text color to red
					}
					else {
						std::cout << "\033[32m";  // Set the text color to green
					}
					std::cout << "    Category: " << getCategoryName(categoryAnalysis.category) << ", Severity: "
						<< static_cast<int>(categoryAnalysis.severity) << std::endl;
					std::cout << "\033[0m";  // Reset the text color
				}
				auto analyzeEnd = std::chrono::high_resolution_clock::now();
				auto analyzeTextDuration = std::chrono::duration_cast<std::chrono::milliseconds>(analyzeEnd - analyzeStart);
				std::cout << "Analyze Image duration: " << analyzeTextDuration.count() << " milliseconds" << std::endl;
				std::cout << "--------------------------------------------------------------------------------------"
					<< std::endl;
				auto endTime = std::chrono::high_resolution_clock::now();
			}
			catch (const std::exception& e) {
				std::cerr << "Error processing file " << filePath << ": " << e.what() << std::endl;
			}
		}
	}

	std::cout << "Press any key to continue..";
	_getch();
}
