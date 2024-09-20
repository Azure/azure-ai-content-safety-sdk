# Azure AI Content Safety embedded SDK samples in C++

These samples demonstrate Azure AI Content Safety embedded SDK for C++.

Azure AI Content Safety embedded SDK enables offline (on-device) processing of images and text and flagging content that is potentially offensive, risky, or otherwise undesirable.

## Prerequisites

Requirements specific to Azure AI Content Safety embedded SDK samples are as follows.
* Supported operating systems and architectures:
  * Windows - `x64`.
* If using Visual Studio (Windows):
  * [Microsoft Visual Studio 2022 or newer](https://www.visualstudio.com/).

## Other dependencies
* onnxruntime 1.18.0, cmake/onnxruntime.cmake
* opencv 4.10.0, cmake/opencv.cmake
* directml 1.15.0, cmake/directml.cmake

## Build the sample

**By building this sample you will download the Microsoft Azure AI Content Safety embedded SDK. By downloading you acknowledge its license, see [Azure AI Content Safety embedded SDK license agreement](//license agreement link).**

[Download the sample code to your development PC.](// GitHub link for sample project)

To tailor the sample to your configuration:
* Edit corresponding `config.ini` for the intended use case text/image before building the sample and update the values of the variables listed below.

Sample settings:
1. `gpuEnabled`
   * Set as **true** to enable gpu, otherwise set to **false**.
1. `gpuDeviceId`
   * Device ID of the GPU, if `gpuEnabled` is true.
1. `numThreads`
   * Number of threads to be used in a multi-threaded environment.
   * We support a maximum number of 4 threads.
1. `licenseText`
   * License text for model initiation.
1. `modelDirectory`
   * Path to the local embedded text/image analysis model(s) on the device file system.
     This may be a single model folder or a top-level folder for several models.
   * Use an absolute path or a path relative to the application working folder.
     The path is recursively searched for model files.
   * Files belonging to a specific model must be present as normal individual files in a model folder,
     not inside an archive, and they must be readable by the application process.
     The model internal subfolder structure must be intact i.e. as originally delivered.
1. `modelName`
   * Name of the embedded text/image analysis model to be used for processing.
   * The models can be in ONNX or MODEL format.
1. `spmModelName`
   * Name of the embedded model for text tokenization, for example SentencePiece model.
   * Required only for text analysis use case.
1. `inputImagesDirectory`
   * Path to the local folder containing the images to be analysed and processed.
   * Required only for image analysis use case.
   * Before running the image analysis demo, this folder must be created and sample images should be placed inside it.
1. `inputTextDirectory`
   * Required only for text analysis use case.
   * Path to the directory containing all the files related to text analysis use case input.
   * All the files like the input text files with strings to be processed and the csv files with block list should be placed inside this folder.
   * Before running the text analysis demo, this folder must be present with all the required input files in it.
1. `inputTextFile`
   * Name of the text file containing input strings to be analysed and processed.
   * This file should always be present inside `inputTextDirectory` folder only.
   * Each sample string should be placed in a new line in the file.
   * Required only for text analysis use case.
   * Before running the text analysis demo, this file must be added and sample strings should be placed in it.
1. `inputWithBlockListTextFile`
   * Name of the text file containing input strings to be analysed and processed with block list.
   * This file should always be present inside `inputTextDirectory` folder only.
   * Each sample string should be placed in a new line in the file.
   * When adding this file, make sure to add **csv file with strings part of block list** in the same folder.
   * More than one csv file with block list strings can be added. The names of these csv files will be used as block list names.
   * Required only for text analysis use case.
   * In order to see output with block list demo, this file must be added and sample strings should be placed in it.


### Visual Studio (Windows)

* Start Microsoft Visual Studio and select **File** \> **Open** \> **Folder**.
* Navigate to the folder for this sample and select it.
* Add a new folder for storing the text/image analysis models as updated in the **config.ini** `modelDirectory`  and save all the models in it. [Sample models](https://msazure.visualstudio.com/Cognitive%20Services/_git/Service-ContentModerator-Embedded?path=/models)
* Update the config.ini file for required text/image use case with the path and names for the corresponding models.
* Compile the complete project using **Build** \> **Build All(Ctrl+Shift+B) / Rebuild All**
  * If there is an error "*Could not find a part of the path*" or "*MSB6003*" while SDK packages are installed, copy/move this sample folder to the storage drive root or close to it, so that folder and file paths become shorter.
* Set the active solution configuration and platform to the desired values under **Build** \> **Configuration Manager** \> **EmbeddedAACSTextDemo.exe/EmbeddedAACSImageDemo.exe**.

## Run the sample

### Visual Studio

* Make sure the required build configuration selected **EmbeddedAACSTextDemo.exe/EmbeddedAACSImageDemo.exe**.
* For text use case, make sure the **text\config.ini** is modified and the `inputTextDirectory`, `inputWithBlockListTextFile`, `inputWithBlockListTextFile` are set properly.
* For image use case, make sure the **text\config.ini** is modified and the `inputImagesDirectory` is set properly with sample images present inside it.
* To debug the sample application and then run it, press F5 or use **Debug** \> **Start Debugging**.
* To run the application without debugging, press Ctrl+F5 or use **Debug** \> **Start Without Debugging**.
* The application will run and display the result in a new console window.


## Remarks

For a new project from scratch,
* Make sure to create a folder for the models and store the required models in it before running the sample project.
* Update the **config.ini** files with the required details for gpu, model path and model names.
