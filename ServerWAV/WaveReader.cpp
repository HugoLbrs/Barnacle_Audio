#include "Wave/WaveReader.hpp"

// Constructor/Destructor
WaveReader::WaveReader() :
	_isOpen(false),
	_waveFormat{0},
	_indexBeingRead(0),
	_bufferSize(1000)
{
	setBuffers(5, _bufferSize);
	// Wait for set / open 
}
WaveReader::~WaveReader() {
	if(_isOpen)
		close();
}

// Methods
bool WaveReader::open() {
	if(_isOpen)
		return true;
	
	// Need at least one buffer non empty
	if(_bufferSize == 0 || _buffers.empty())
		return false;
	
	// Need to have set the format
	if(_waveFormat.wFormatTag != WAVE_FORMAT_PCM)
		return false;
	
	// Create event
	_hEvent = CreateEvent(0, TRUE, FALSE, 0);
	
	// Open sound card device
	if(waveOutOpen(&_hWaveOut, WAVE_MAPPER, &_waveFormat, (DWORD)_hEvent, 0, CALLBACK_EVENT) != MMSYSERR_NOERROR) {
		close();
		return false;
	}
	
	ResetEvent(_hEvent);
	
	// Begin queue
	for(auto& buffer : _buffers) {
		waveOutPrepareHeader(_hWaveOut, &buffer.header, sizeof(WAVEHDR));
		waveOutWrite(_hWaveOut, &buffer.header, sizeof(WAVEHDR));
	}
	
	_isOpen = true;
	_indexBeingRead = 0;
	
	return true;
}
bool WaveReader::close() {
	if(!_isOpen)
		return true;
	
	ResetEvent(_hEvent);
	
	// Unqueue
	for(auto& buffer : _buffers) 
		waveOutUnprepareHeader(_hWaveOut, &buffer.header, sizeof(WAVEHDR));
	
	waveOutClose(_hWaveOut);
	CloseHandle(_hEvent);
	
	_isOpen = false;
	
	return true;
}
bool WaveReader::queueData(const WaveData& wavData) {
	// Wait to have available buffer
	WaitForSingleObject(_hEvent, INFINITE);
	ResetEvent(_hEvent);

	// Copy
	WaveData& wavDataBuf = _buffers[_indexBeingRead];
	wavDataBuf.copy(wavData);
	
	// Load
	waveOutWrite(_hWaveOut, &wavDataBuf.header, sizeof(WAVEHDR));

	// Change index
	_indexBeingRead = (_indexBeingRead+1) % _buffers.size();
		
	return true;
}
	
// Setters
void WaveReader::setFormat(int nchannels, int sampleRate, int bitsPerSample) {
	// Already open, cannot change
	if(_isOpen)
		return;
	
	_waveFormat.wFormatTag 		= WAVE_FORMAT_PCM;
	_waveFormat.nChannels 			= nchannels;
	_waveFormat.nSamplesPerSec 	= sampleRate;
	_waveFormat.wBitsPerSample 	= bitsPerSample;
	_waveFormat.nBlockAlign 		= nchannels * bitsPerSample / 8;
	_waveFormat.nAvgBytesPerSec	= sampleRate * _waveFormat.nBlockAlign;
	_waveFormat.cbSize 				= 0;
}
void WaveReader::setBuffers(size_t nbBuffer, size_t bufferSize) {
	// Already open, cannot change
	if(_isOpen)
		return;
	
	_bufferSize = bufferSize;
	_buffers.resize(nbBuffer);
	
	for(size_t i = 0; i < nbBuffer; i++)
		_buffers[i].setSize(bufferSize);
}
	
// Getters
size_t WaveReader::getBufferSize() const {
	return _bufferSize;
}
size_t WaveReader::getNumberBuffer() const {
	return _buffers.size();
}