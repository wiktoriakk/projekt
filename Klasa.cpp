#include "Klasa.h"
#include<iostream>
#include<curl.h>
#include "json.hpp"
#include <stdexcept>
#include <sstream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


std::string Klasa::getWeather(const std::string& city, const std::string& apiKey, bool isDaily) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url;
        if (isDaily) {
            url = "http://api.weatherapi.com/v1/forecast.json?key=" + apiKey + "&q=" + city + "&days=1&aqi=no&alerts=no";
        }
        else {
            url = "http://api.weatherapi.com/v1/forecast.json?key=" + apiKey + "&q=" + city + "&days=14&aqi=no&alerts=no";
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            throw std::runtime_error("Failed to fetch weather data: " + std::string(curl_easy_strerror(res)));
        }
        curl_easy_cleanup(curl);
    }
    else {
        throw std::runtime_error("Failed do initalize CURL");
    }
    try {
        auto json = json::parse(readBuffer);
        std::ostringstream oss;

        if (isDaily) {
            auto forecast = json["forecast"]["forecastday"][0]["hour"];
            for (const auto& hour : forecast) {
                std::string time = hour["time"].get<std::string>();
                double temperature = hour["temp_c"];
                double humidity = hour["humidity"];
                double wind_kph = hour["wind_kph"];
                std::string condition = hour["condition"]["text"];

                oss << "Time: " << time << "\r\n"
                    << "Temperature: " << temperature << " °C\r\n"
                    << "Humidity: " << humidity << "%\r\n"
                    << "Wind Speed: " << wind_kph << " kph\r\n"
                    << "Condition: " << condition << "\r\n\n";
            }
        }
        else {
            auto forecast = json["forecast"]["forecastday"];
            for (const auto& day : forecast) {
                std::string date = day["date"].get<std::string>();
                double avgTemp = day["day"]["avgtemp_c"];
                double maxTemp = day["day"]["maxtemp_c"];
                double minTemp = day["day"]["mintemp_c"];
                std::string condition = day["day"]["condition"]["text"];

                oss << "Date: " << date << "\r\n"
                    << "Average Temperature: " << avgTemp << " °C\r\n"
                    << "Max Temperature: " << maxTemp << " °C\r\n"
                    << "Min Temperature: " << minTemp << " °C\r\n"
                    << "Condition: " << condition << "\r\n\n";
            }
        }
        return oss.str();
    }
    catch (const json::exception& e) {
        throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
    }
}
