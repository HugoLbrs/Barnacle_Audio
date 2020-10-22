#include "Wave/WaveFile.hpp"

// Constructor
WaveFile::WaveFile(const std::string&  pathFile) :
	_pathFile(pathFile),
	_valide(false),
	_header{0},
	_fileIn(pathFile, std::ios::binary),
	_fileSize(0)
{
	// Check file
	if(!_fileIn.is_open())
		return; // Abort
	
	// Size file
	_fileIn.seekg(0, _fileIn.end);
	_fileSize = _fileIn.tellg();
	_fileIn.seekg(0, _fileIn.beg);
	
	if(_fileSize < 44)
		return; // Abort
	
	// Read header
	_fileIn.read(&_header[0], 44);
	
	// Validity
	_valide = 
		get(chunkID) == _base256("RIFF",  false) &&
		get(audioFormat) == 1; // PCM
}


// Destructor
WaveFile::~WaveFile() {
	_valide = false;
	
	if(_fileIn)
		_fileIn.close();
}

// Public Methods
const bool WaveFile::isValide() const {
	return _valide;
}

const bool WaveFile::readSamples(WaveData* waveData) {
	if(!_valide || !waveData)
		return false;
	
	// Check file
	if(!_fileIn) { // End reached, reboot
		_fileIn = std::ifstream(_pathFile, std::ios::binary);
		
		// Error ?
		if(!_fileIn.is_open()) {
			_valide = false;
			return false;
		}		
		
		_fileIn.seekg(44, _fileIn.beg);
	}
	
	// Read
	_fileIn.read(waveData->getData(), waveData->getSizeMax());
		
	// Ok
	size_t bytesRead = _fileIn ? waveData->getSizeMax() : (size_t)(_fileIn.gcount());
	waveData->header.dwBufferLength = (DWORD)bytesRead;
	
	return bytesRead > 0;
}

// Getters
const int WaveFile::get(const FieldName& fieldname) const {
	// Field exists
	if(_headerPlan.fields.find(fieldname) == _headerPlan.fields.end())
		return 0;
	
	// Content
	auto element = _headerPlan.fields.at(fieldname);
	return _base256(&_header[element.offset], element.size, element.littleEndian);
}

// Private methods
int WaveFile::_base256(const char* tab, const size_t n, bool littleEndian)  const {
	int res = 0;	
	
	for(size_t i = 0; i < n; i++)
		res = (res<<8) + (int)(unsigned char)tab[littleEndian ? n - i - 1 : i];
	
	return res;	
}
int WaveFile::_base256(const std::string& str, bool littleEndian)  const {
	return _base256(str.c_str(), str.size(), littleEndian);
}