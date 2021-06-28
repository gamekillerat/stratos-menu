#include "httpRequest.h"
#include "common.hpp"
#include <chrono>
#include "Auth/Networking/sha512.hh"

#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

#define FOR_PUBLISH_UNMARK_THIS

using namespace std;

#pragma comment(lib, "ws2_32.lib")

namespace Big {

	namespace convert
	{
		wstring s2ws(const std::string& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r;
		}

		LPCWSTR lpcwstr(const std::string& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r.c_str();
		}
	}

	namespace auth {

		string Get_Volume_Information()
		{
			WCHAR volumeName[MAX_PATH + 1] = { 0 };
			WCHAR fileSystemName[MAX_PATH + 1] = { 0 };
			DWORD serialNumber = 0;
			DWORD maxComponentLen = 0;
			DWORD fileSystemFlags = 0;
			LPCSTR C_PATH = "C:\\";
			GetVolumeInformation(C_PATH, (LPSTR)volumeName, ARRAYSIZE(volumeName), &serialNumber, &maxComponentLen, &fileSystemFlags, (LPSTR)fileSystemName, sizeof(fileSystemName));
			string returnValue;
			string leng1 = to_string(maxComponentLen);
			string leng2 = to_string(serialNumber);
			wstring ws(fileSystemName);
			string str(ws.begin(), ws.end());
			string reCon = str; //resultetConvert;
			returnValue = leng1 + leng2 + reCon;

			return returnValue;
		}

		string sha512(string value)
		{
			return sw::sha512::calculate(value).c_str();
		}

		string get_hwid()
		{
			string conv = Get_Volume_Information();
			conv = sha512(conv);

			return sha512(conv);
		}

		string ReadStringFromIni(string file, string app, string key)
		{
			char buf[500];
			GetPrivateProfileStringA(app.c_str(), key.c_str(), "NULL", buf, 500, file.c_str());
			return (std::string)buf;
		}

		string read_login_data(string key)
		{
			string filepath = getenv("appdata");
			filepath += "/Stratos/Login.login";
			return ReadStringFromIni(filepath, "StratosLogin", key);
		}

		std::string string_to_lower(std::string text) {
			std::transform(text.begin(), text.end(), text.begin(), [](unsigned char lowerThis) { return std::tolower(lowerThis); });
			return text;
		}

		std::string string_to_upper(std::string text) {
			std::transform(text.begin(), text.end(), text.begin(), [](unsigned char upperThis) { return std::toupper(upperThis); });
			return text;
		}

		bool containsWord(std::string StringOrArray, std::string SearchForThisWord)
		{
			StringOrArray = string_to_lower(StringOrArray);
			SearchForThisWord = string_to_lower(SearchForThisWord);
			std::string::size_type find = StringOrArray.find(SearchForThisWord);
			if (find == std::string::npos) {
				return false;
			}
			else {
				return true;
			}
			return false;
		}

		string replaceAll(string subject, const string& search,
			const string& replace) {
			size_t pos = 0;
			while ((pos = subject.find(search, pos)) != string::npos) {
				subject.replace(pos, search.length(), replace);
				pos += replace.length();
			}
			return subject;
		}

		string DownloadString(string URL) {
			HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
			HINTERNET urlFile;
			string rtn;
			if (interwebs) {
				urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
				if (urlFile) {
					char buffer[2000];
					DWORD bytesRead;
					do {
						InternetReadFile(urlFile, buffer, 2000, &bytesRead);
						rtn.append(buffer, bytesRead);
						memset(buffer, 0, 2000);
					} while (bytesRead);
					InternetCloseHandle(interwebs);
					InternetCloseHandle(urlFile);
					string p = replaceAll(rtn, "|n", "\r\n");
					return p;
				}
			}
			InternetCloseHandle(interwebs);
			string p = replaceAll(rtn, "|n", "\r\n");
			return p;
		}

		void login()
		{

#ifdef FOR_PUBLISH_UNMARK_THIS // kweet nie begint een beetje saai te worden van de haxxa te spelen hihi

			string username = read_login_data("username");
			string password = sha512(read_login_data("password"));
			//g_Logger->Info(sha512(read_login_data("password")).c_str());
			string hwid = get_hwid();
			string param = "S" + base64_encode(username + ";" + password + ";" + hwid, true);
			string linkString = "http://stratos-menu.xyz/loginMenu.php?0xF4WE=" + param;
			/*auto link = fmt::format(linkString);
			http::Request request(link);

			http::Response response = request.send("GET");

			std::string str(response.body.begin(), response.body.end());

			if (str != "sdnfhnbsdjhfbhjdshbfhjdsbfhdsfbhsdbfhjsdbfjhsd")
			{
				g_Logger->Info(str.c_str());
				g_Logger->Info("0xD4AS6F4AS68");

				/*exit(0);

				system("taskkill /f /im GTAV.exe");
				system("taskkill /f /im GTA5.exe");
				system("taskkill /f /im GTA V.exe");
				system("taskkill /f /im GTA 5.exe");
				system("taskkill /f /im GTA_V.exe");
				system("taskkill /f /im GTA_5.exe");
				system("taskkill /f /im GTA5");
				system("taskkill /f /im GTAV");
				system("taskkill /f /im Grand Theft Auto 5");
				system("taskkill /f /im Grand Theft Auto V");
				system("taskkill /f /im Grand Theft AutoV");
				system("taskkill /f /im Grand Theft Auto5");

				g_Running = false;*/
				//}
			using std::chrono::high_resolution_clock;
			using std::chrono::duration_cast;
			using std::chrono::duration;
			using std::chrono::milliseconds;

			auto TimeStart = high_resolution_clock::now();
			string result = DownloadString(linkString);
			auto TimeEnd = high_resolution_clock::now();

			auto elapsed = duration_cast<milliseconds>(TimeEnd - TimeStart);

			if (elapsed.count() > 10) {
				g_Logger->Info("0x55DS5DS6");

				exit(0); //,

				system("taskkill /f /im GTAV.exe");
				system("taskkill /f /im GTA5.exe");
				system("taskkill /f /im GTA V.exe");
				system("taskkill /f /im GTA 5.exe");
				system("taskkill /f /im GTA_V.exe");
				system("taskkill /f /im GTA_5.exe");
				system("taskkill /f /im GTA5");
				system("taskkill /f /im GTAV");
				system("taskkill /f /im Grand Theft Auto 5");
				system("taskkill /f /im Grand Theft Auto V");
				system("taskkill /f /im Grand Theft AutoV");
				system("taskkill /f /im Grand Theft Auto5");

				g_Running = false;
			}

			//if (result == "sdnfhnbsdjhfbhjdshbfhjdsbfhdsfbhsdbfhjsdbfjhsd")
			//if (containsWord(result, "jhsdnfhnbsdjhfbhjdshbfhjdsbfhdsfbhsdbfhjsdb") &&

			char key_arr[] = { 106, 104, 115, 100, 110, 102, 104, 110, 98, 115, 100, 106, 104, 102, 98, 104, 106, 100, 115, 104, 98, 102, 104, 106, 100, 115, 98, 102, 104, 100, 115, 102, 98, 104, 115, 100, 98, 102, 104, 106, 115, 100, 98 };
			string key = "";
			for (char key_char : key_arr)
			{
				key += key_char;
			}

			if (containsWord(result, key) && //"jhsdnfhnbsdjhfbhjdshbfhjdsbfhdsfbhsdbfhjsdb"
				std::count(result.begin(), result.end(), 'j') >= 7 &&
				std::count(result.begin(), result.end(), 'j') < 9 &&
				std::count(result.begin(), result.end(), 'k') >= 3 &&
				std::count(result.begin(), result.end(), 'k') < 5 &&
				std::count(result.begin(), result.end(), 'n') >= 3 &&
				std::count(result.begin(), result.end(), 'n') < 5)
			{
				//g_Logger->Info(("Logged In" + result).c_str());
			}
			else
			{
				g_Logger->Info(result.c_str());
				g_Logger->Info("0x55DS5DS6");

				exit(0);

				system("taskkill /f /im GTAV.exe");
				system("taskkill /f /im GTA5.exe");
				system("taskkill /f /im GTA V.exe");
				system("taskkill /f /im GTA 5.exe");
				system("taskkill /f /im GTA_V.exe");
				system("taskkill /f /im GTA_5.exe");
				system("taskkill /f /im GTA5");
				system("taskkill /f /im GTAV");
				system("taskkill /f /im Grand Theft Auto 5");
				system("taskkill /f /im Grand Theft Auto V");
				system("taskkill /f /im Grand Theft AutoV");
				system("taskkill /f /im Grand Theft Auto5");

				g_Running = false;
			}
#endif
		}
	}
}