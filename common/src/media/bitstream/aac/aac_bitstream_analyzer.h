#pragma once
#include <cstdint>

class AACBitstreamAnalyzer
{
public:
	static bool IsValidAdtsUnit(const uint8_t *payload);

private:

};