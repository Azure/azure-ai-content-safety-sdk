#include <azure_ai_contentsafety_text.h>
#include <iostream>
#include <csignal>
#include "thread"
#include <filesystem>
#include <fstream>
#include <functional>
#include <cstdlib>  // For _dupenv_s
#include <string>   // For std::string
#include <stdexcept> // For std::invalid_argument, std::out_of_range
#include <map>
#include <sstream>
#include <Windows.h>
#include <conio.h>

using namespace Azure::AI::ContentSafety;

std::map<std::string, std::string> readConfig(const std::string &filename) {
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

std::string getCategoryName(TextCategory category) {
    switch (category) {
        case TextCategory::Hate:
            return "Hate";
        case TextCategory::SelfHarm:
            return "Self Harm";
        case TextCategory::Sexual:
            return "Sexual";
        case TextCategory::Violence:
            return "Violence";
        default:
            return "Unknown";
    }
}

std::vector<char> readFile(const std::string& filename) {
    std::vector<char> buffer;
#ifdef _WIN32
    HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Could not open file" << std::endl;
        return buffer;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        std::cerr << "Could not get file size" << std::endl;
        CloseHandle(hFile);
        return buffer;
    }

    HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object" << std::endl;
        CloseHandle(hFile);
        return buffer;
    }

    char* data = (char*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if (data == NULL) {
        std::cerr << "Could not map view of file" << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return buffer;
    }

    buffer.assign(data, data + fileSize);

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
#else
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Could not open file" << std::endl;
        return buffer;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Could not get file size" << std::endl;
        close(fd);
        return buffer;
    }

    char* data = static_cast<char*>(mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED) {
        std::cerr << "Could not map file" << std::endl;
        close(fd);
        return buffer;
    }

    buffer.assign(data, data + sb.st_size);

    munmap(data, sb.st_size);
    close(fd);
#endif
    return buffer;
}

void Init(TextModelRuntime **aacs, std::map<std::string, std::string> config) {
    std::string licenseText = config["licenseText"];

    TextModelConfig aacsConfig;
    aacsConfig.gpuEnabled = (config["gpuEnabled"] == "true");
    aacsConfig.gpuDeviceId = std::stoi(config["gpuDeviceId"]);
    aacsConfig.numThreads = std::stoi(config["numThreads"]);
    aacsConfig.modelDirectory = config["modelDirectory"];
    aacsConfig.modelName = config["modelName"];
    aacsConfig.spmModelName = config["spmModelName"];
    std::cout << "gpuEnabled: " << aacsConfig.gpuEnabled << std::endl;
    std::cout << "gpuDeviceId: " << aacsConfig.gpuDeviceId << std::endl;
    std::cout << "numThreads: " << aacsConfig.numThreads << std::endl;
    std::cout << "modelDirectory: " << aacsConfig.modelDirectory << std::endl;
    std::cout << "modelName: " << aacsConfig.modelName << std::endl;
    std::cout << "spmModelName: " << aacsConfig.spmModelName << std::endl;

    (*aacs) = new TextModelRuntime(licenseText.c_str(), aacsConfig);
    try {
        (*aacs)->Reload();
    } catch (const std::exception &ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }
}

void processInputText(TextModelRuntime* aacs, std::string inputText) {
    AnalyzeTextOptions request;
    std::cout << "  Your input: " << inputText << std::endl;
    request.text = inputText;
    std::cout << "  AnalyzeResult: " << std::endl;
    auto severityThreshold = 3;
    // Run inference
    auto analyzeStart = std::chrono::high_resolution_clock::now();
    auto result = aacs->AnalyzeText(request);
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
    std::cout << "AnalyzeText duration: " << analyzeTextDuration.count() << " milliseconds" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------"
        << std::endl;
}

void processInputTextWithBlockList(TextModelRuntime* aacs, std::string inputText, std::vector<std::string> blocklist_names) {
    AnalyzeTextOptions request;
    std::cout << "  Your input: " << inputText << std::endl;
    request.text = inputText;
    request.blocklistNames = blocklist_names;
    std::cout << "  AnalyzeResult: " << std::endl;
    auto severityThreshold = 3;
    // Run inference
    auto analyzeStart = std::chrono::high_resolution_clock::now();
    auto result = aacs->AnalyzeText(request);
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

    std::cout << "  BlockList Matches : " << result->blocklistsMatched.size() << std::endl;
    for (const auto& blockListItem : result->blocklistsMatched) {
        std::cout << "\033[31m";  // Set the text color to red
        std::cout << "    BlockList Name: " << blockListItem.blocklistName << ", Text: "
            << blockListItem.blocklistItemText << std::endl;
        std::cout << "\033[0m";  // Reset the text color
    }

    auto analyzeEnd = std::chrono::high_resolution_clock::now();
    auto analyzeTextDuration = std::chrono::duration_cast<std::chrono::milliseconds>(analyzeEnd - analyzeStart);
    std::cout << "AnalyzeText duration: " << analyzeTextDuration.count() << " milliseconds" << std::endl;
    std::cout << "--------------------------------------------------------------------------------------"
        << std::endl;
}

void processInputFile(TextModelRuntime* aacs, const std::string& inputDirectory, const std::string& fileName) {
    auto filePath = inputDirectory + "\\" + fileName;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "processInputFile, Could not open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        processInputText(aacs, line);
    }

    file.close();
}


void processInputFileWithBlockList(TextModelRuntime* aacs, const std::string& inputDirectory, const std::string& fileName) {
    auto filePath = inputDirectory + "\\" + fileName;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "processInputFileWithBlockList, Could not open file: " << filePath << std::endl;
        return;
    }

    std::vector<std::string> blocklist_names;
    for (const auto& entry : std::filesystem::directory_iterator(inputDirectory)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".csv") {
                std::vector<char> buffer = readFile(entry.path().string());
                auto blockListName = entry.path().stem().string();
                aacs->AddBlocklist(blockListName, buffer.data(), buffer.size());
                blocklist_names.push_back(blockListName);
                std::cout << "processInputFileWithBlockList, adding block list : " << blockListName << std::endl;
            }
        }
    }

    std::string line;
    while (std::getline(file, line)) {
        processInputTextWithBlockList(aacs, line, blocklist_names);
    }

    file.close();
}

void processSampleInputFiles(TextModelRuntime *aacs, const std::string& inputDirectory, const std::string& inputFileName, const std::string& inputWithBlockListFileName) {
    processInputFile(aacs, inputDirectory, inputFileName);
    processInputFileWithBlockList(aacs, inputDirectory, inputWithBlockListFileName);
}

int main(int argc, char *argv[]) {
    std::map<std::string, std::string> config = readConfig("config.ini");
    TextModelRuntime *aacs = NULL;
    Init(&aacs, config);

    processSampleInputFiles(aacs, config["inputTextDirectory"], config["inputTextFile"], config["inputWithBlockListTextFile"]);

    std::cout << "Press any key to continue..";
    _getch();
    return 0;
}

