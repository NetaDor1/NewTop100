#include "CommonObject.h"

//Declares the DownloadThread class which contains an overloaded operator() function to handle
//  downloading movie data and a SetUrl function to set the URL for downloading data.

class DownloadThread {

public:
	void operator()(CommonObjects& common);
	void SetUrl(std::string_view new_url);
private:
	std::string _download_url;

};