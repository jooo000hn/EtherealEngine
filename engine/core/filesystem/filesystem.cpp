#include "filesystem.h"
#include "../common/platform_config.h"
#include "../common/string.h"

namespace fs
{

bool add_path_protocol(const std::string& protocol, const path& dir)
{
	// Protocol matching is case insensitive, convert to lower case
	auto protocol_lower = string_utils::to_lower(protocol);

	auto& protocols = get_path_protocols();
	// Add to the list
	protocols[protocol_lower] = dir.string();

	// Success!
	return true;
}

protocols_t& get_path_protocols()
{
	static protocols_t protocols;
	return protocols;
}

byte_array_t read_stream(std::istream& stream)
{
	// Open the stream
	byte_array_t read_memory;
	if(stream.good())
	{
		// get length of file:
		stream.seekg(0, stream.end);
		size_t length = static_cast<std::size_t>(stream.tellg());
		stream.seekg(0, stream.beg);

		read_memory.resize(length, '\0'); // reserve space
		char* begin = (char*)&*read_memory.begin();

		stream.read(begin, length);

		stream.clear();
		stream.seekg(0);
	}
	// Done
	return read_memory;
}

path resolve_protocol(const path& _path)
{
	const auto string_path = _path.generic_string();
	auto pos = string_path.find(':', 0) + 1;
	if(pos == std::string::npos)
		return path{};

	const auto root = string_path.substr(0, pos);

	fs::path relative_path = string_path.substr(pos + 1);
	// Matching path protocol in our list?
	auto& protocols = get_path_protocols();

	auto it = protocols.find(root);

	if(it == std::end(protocols))
		return path{};

	const auto resolved = it->second;
	auto result = resolved / relative_path.make_preferred();
	return result;
}
}

#include <stdio.h>
#include <stdlib.h>

namespace fs
{
path executable_path_fallback(const char* argv0)
{
	if(0 == argv0 || 0 == argv0[0])
	{
		return "";
	}
	fs::error_code err;
	path full_path(system_complete(path(std::string(argv0)), err));
	return full_path;
}
}
#if $on($windows)
#include <Windows.h>
namespace fs
{
path executable_path(const char* argv0)
{
	char buf[1024] = {0};
	DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
	if(ret == 0 || ret == sizeof(buf))
	{
		return executable_path_fallback(argv0);
	}
	return path(std::string(buf));
}
void show_in_graphical_env(const path& _path)
{
	ShellExecuteA(NULL, NULL, _path.string().c_str(), NULL, NULL, SW_SHOWNORMAL);
}
}
#elif $on($apple)
#include <mach-o/dyld.h>
namespace fs
{
path executable_path(const char* argv0)
{
	char buf[1024] = {0};
	uint32_t size = sizeof(buf);
	int ret = _NSGetExecutablePath(buf, &size);
	if(0 != ret)
	{
		return executable_path_fallback(argv0);
	}
	fs::error_code err;
	path full_path(system_complete(path(std::string(buf)).normalize(), err));
	return full_path;
}
void show_in_graphical_env(const path& _path)
{
}
}
#elif $on($linux)

#include <unistd.h>
namespace fs
{
path executable_path(const char* argv0)
{
	char buf[1024] = {0};
	ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
	if(size == 0 || size == sizeof(buf))
	{
		return executable_path_fallback(argv0);
	}
	std::string p(buf, size);
	fs::error_code err;
	path full_path(system_complete(path(p).normalize(), err));
	return full_path;
}
void show_in_graphical_env(const path& _path)
{
	static std::string cmd = "xdg-open";
	static std::string space = " ";
	const std::string cmd_args = _path.string();
	const std::string whole_command = cmd + space + cmd_args;
	system(whole_command.c_str());
}
}
#else
namespace fs
{
path executable_path(const char* argv0)
{
	return executable_path_fallback(argv0);
}

void show_in_graphical_env(const path& _path)
{
}
}
#endif
