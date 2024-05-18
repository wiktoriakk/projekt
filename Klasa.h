#pragma once
#include "API.h"
#include <string>

class EXPORT_API Klasa
{
public:
	std::string getWeather(const std::string& city, const std::string& apiKey);
};
