#pragma once

#include <map>
#include <string>
#include <vector>
#include <algorithm>

#ifndef VARTYPE_NUMERICAL
#define VARTYPE_NUMERICAL			10301
#define VARTYPE_NUMERICAL_INT		103011
#define VARTYPE_NUMERICAL_DOUBLE	103012
#define VARTYPE_CATEGORICAL_GROUP			10302	// group information (categorical) if (# of items > # of categorical values)
#define VARTYPE_CATEGORICAL_INDIVIDUAL		10303	// individual information (categorical) if (# of items == # of categorical values)
#endif

namespace CSVParser 
{
	int LoadingCSVFile(const char* path, std::vector<std::string>& data);
	void csvline_populate_values(std::vector<std::string>& record, std::string line, std::string delimiter);
	int ParsingFirstFiveLinesData(std::vector<std::string>& data,
		bool& bVariableNamesExistInData,
		std::vector<std::vector<std::string>>& datainstances,
		std::map<int, UINT>& attributetypes);
	int ParsingRestOfLinesData(std::vector<std::string>& data,
		std::vector<std::vector<std::string>>& datainstances);
};