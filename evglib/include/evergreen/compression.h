#pragma once

// Stateful compression/decompression machine
class Compressor
{
public:
	enum Direction
	{
		None = 0,
		Input = 1,
		Output = 2,
		Both = 3,
	};

	virtual UInt32 id() = 0;

	virtual bool eof() = 0;

	virtual bool fail() = 0;

	virtual Compressor& operator<<(char lhs) = 0;



};







class ZlibCompressorInput
{
public:
	z_stream_s istream;


};

// Literally just zlib
template <Compressor::Direction direction = Compressor::Direction::Both, Int level >
class ZlibCompressor : public Compressor
{
public:
	
	z_stream_s ostream;


	ZlibCompressor(const Direction _direction, const Int _level = 9)
	{
		
	}

	
	virtual UInt32 id() { return 1; }

	virtual bool eof() = 0;

	virtual bool fail() = 0;

	virtual Compressor& operator<<(char lhs) = 0;
	{


		return *this;
	}
};