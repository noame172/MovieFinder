#include "multiServer.h"
#include "WSAInitializer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

void loadEnvFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ENV WARNING] Could not open " << filename << ". Relying on system env." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // דילוג על שורות ריקות או הערות
        if (line.empty() || line[0] == '#') continue;

        // מציאת המיקום של ה- '='
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            // הסרת רווחים מיותרים אם יש
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));

            // הזרקת המשתנה למערכת עבור התוכנית הנוכחית (תואם Windows)
            _putenv_s(key.c_str(), value.c_str());
        }
    }
    file.close();
    std::cout << "[ENV SUCCESS] Environment variables loaded from " << filename << std::endl;
}

int main()
{
	try
	{
		loadEnvFile("api_key.env");
		WSAInitializer wsaInit;
		Server server;
		server.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}
	return 0;
}