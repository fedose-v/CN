#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/stat.h>


class CacheManager
{
public:
	CacheManager()
	{
		struct stat st = { 0 };
		if (stat("cache", &st) == -1)
		{
			mkdir("cache", 0700);
		}
	}

	static bool IsCached(const std::string& url)
	{
		std::string filename = GenerateFilename(url);
		std::ifstream file(filename);
		return file.good();
	}

	static std::vector<char> Retrieve(const std::string& url)
	{
		std::string filename = GenerateFilename(url);
		std::ifstream file(filename, std::ios::binary);
		std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return buffer;
	}

	static void Store(const std::string& url, const std::vector<char>& data)
	{
		std::string filename = GenerateFilename(url);
		std::ofstream file(filename, std::ios::binary);
		if (file.is_open())
		{
			file.write(data.data(), data.size());
			file.close();
		}
	}

private:
	static std::string GenerateFilename(const std::string& url)
	{
		std::hash<std::string> hasher;
		size_t hash = hasher(url);
		return "cache/" + std::to_string(hash);
	}
};