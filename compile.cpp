#include <iostream>
#include <fstream>
#include <filesystem>

#include <shaderc/shaderc.hpp>


void compile_shader(const std::filesystem::path& path, const shaderc::CompileOptions& options)
{
	if (!std::filesystem::exists(path)) 
	{
		std::cerr << "File " << path.filename() << " does not exist... skipping." << std::endl;
		return;
	}

	const std::string ext = path.extension().string();

	shaderc_shader_kind kind;
	if (ext == ".vert")
		kind = shaderc_vertex_shader;
	else if (ext == ".frag")
		kind = shaderc_fragment_shader;
	else
	{
		std::cerr << "File " << path.filename() << " has unsupported extension... skipping." << std::endl;
		return;
	}

	std::ifstream input_file(path, std::ios::in | std::ios::ate);

	if (!input_file.is_open())
	{
		std::cerr << "File " << path.filename() << " couldn't be opened... skipping." << std::endl;
		return;
	}

	int size = input_file.tellg();
	input_file.seekg(0);

	std::string source;
	source.resize(size);
	input_file.read(source.data(), size);

	input_file.close();

	shaderc::Compiler compiler;
	const shaderc::CompilationResult result = compiler.CompileGlslToSpv(source.c_str(), kind, path.filename().string().c_str(), options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << result.GetErrorMessage();
		return;
	}

	const std::vector<uint32_t> buffer(result.begin(), result.end());

	std::string output_filename = path.string() + ".spv";
	std::ofstream output_file(output_filename, std::ios::out | std::ios::binary);

	if (!output_file.is_open())
	{
		std::cerr << "File \"" << output_filename << "\" couldn't be opened... skipping." << std::endl;
		return;
	}

	output_file.write((const char*)buffer.data(), buffer.size() * 4);
	output_file.close();

	std::cout << "Done!" << std::endl;
}

int main()
{
	std::filesystem::path shaders_dir{ "../shaders" };
	if (!std::filesystem::exists(shaders_dir))
	{
		std::cerr << "Shaders dir not found" << std::endl;
		return 0;
	}

	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
	options.SetTargetSpirv(shaderc_spirv_version_1_6);
	options.SetSourceLanguage(shaderc_source_language_glsl);

	for (auto entry : std::filesystem::directory_iterator{ shaders_dir })
	{
		if (entry.is_directory() || entry.path().extension() == ".spv")
			continue;

		std::cout << "Compiling " << entry.path().filename() << ":\n";

		compile_shader(entry.path(), options);
	}

	return 0;
}