#pragma once

#include <vector>
class Global
{
private:
	Global();
	virtual ~Global();
public:
	Global * GetIns()
	{
		static Global global;
		return &global;
	}


	//std::vector<>
};

