#pragma once

using namespace std;

namespace Big {
	namespace auth {

		string sha512(string value);

		string get_hwid();

		string ReadStringFromIni(string file, string app, string key);

		string read_login_data(string key);

		void login();
	}

	namespace convert
	{
		wstring s2ws(const std::string& s);

		LPCWSTR lpcwstr(const std::string& s);
	}
}