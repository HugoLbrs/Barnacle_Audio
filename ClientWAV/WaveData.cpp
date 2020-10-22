#include "Wave/WaveData.hpp"

// Constructor
WaveData::WaveData(const size_t bufferSize) : 
	header{0},
	_buffer(bufferSize, 0)
{  
	// Link buffer and header
	header.lpData = &_buffer[0];
	header.dwBufferLength = (DWORD)bufferSize;
}

WaveData::WaveData(const std::string& str) : 
	header{0},
	_buffer(str.begin(), str.end())
{
	// Link buffer and header
	header.lpData = &_buffer[0];
	header.dwBufferLength = (DWORD)_buffer.size();
}

// Methods
void WaveData::copy(const WaveData& wvd) {
	_buffer.assign(wvd._buffer.begin(), wvd._buffer.end()); 
	header.dwBufferLength = wvd.header.dwBufferLength;
}
std::string WaveData::str() const {
	return std::string(_buffer.begin(), _buffer.end());
}

// Setters
void WaveData::setSize(const size_t bufferSize) {
	_buffer = std::vector<char>(bufferSize, 0);
	
	header.dwBufferLength = (DWORD)bufferSize;
	header.lpData = &_buffer[0];
}

// Getters
char* WaveData::getData() { 
	return &_buffer[0];
}

const size_t WaveData::getSizeMax() const {
	return _buffer.size();
}