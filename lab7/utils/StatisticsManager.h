#pragma once
#include <iostream>
#include <mutex>

class StatisticsManager
{
public:
	static void LogRequest(const std::string& url)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		std::cout << "[REQUEST] " << url << std::endl;
	}

	static void LogHit(const std::string& url)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cacheHits++;
		std::cout << "[CACHE HIT] " << url << std::endl;
		PrintStats();
	}

	static void LogMiss(const std::string& url)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cacheMisses++;
		std::cout << "[CACHE MISS] " << url << std::endl;
		PrintStats();
	}

	static void LogError(const std::string& msg)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		std::cerr << "[ERROR] " << msg << std::endl;
	}

private:
	static void PrintStats()
	{
		std::cout << "--- Stats: Hits: " << m_cacheHits
				  << " | Misses: " << m_cacheMisses << " ---" << std::endl;
	}

	static std::mutex m_mutex;
	static int m_cacheHits;
	static int m_cacheMisses;
};

std::mutex StatisticsManager::m_mutex;
int StatisticsManager::m_cacheHits = 0;
int StatisticsManager::m_cacheMisses = 0;