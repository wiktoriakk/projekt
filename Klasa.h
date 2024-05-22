#pragma once
#ifndef KLASA
#define KLASA
#include "API.h"
#include <string>

class EXPORT_API Klasa
{
public:
    std::string getWeather(const std::string& city, const std::string& apiKey, bool isDaily); // Dodana nowa metoda z parametrem trybu prognozy
};

#endif // KLASA
