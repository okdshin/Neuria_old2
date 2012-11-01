#pragma once
//SocketSessionHeader:20121029
#include <iostream>
#include <cstdint>
#include <cmath>
#include <cassert>
#include "../ByteArray.h"

namespace neuria {
namespace network {

using ByteArraySize = uint64_t;

class SocketSessionHeader {
public:
    SocketSessionHeader(){}
    SocketSessionHeader(ByteArraySize byte_array_size) : 
		byte_array_size(byte_array_size){}

	static auto IsEnableParse(const ByteArray& byte_array) -> bool {
		assert(!byte_array.empty());
		uint8_t first_byte = byte_array.front();
		if(first_byte < 253){
			return true;
		}
		else if(first_byte == 253){
			if(byte_array.size() >= 3){
				return true;	
			}
		}
		else if(first_byte == 254){
			if(byte_array.size() >= 5){
				return true;	
			}
		}
		else if(first_byte == 255){
			if(byte_array.size() >= 9){
				return true;	
			}
		}
		return false;
	}

	static auto Parse(ByteArray byte_array) -> SocketSessionHeader {
		assert(IsEnableParse(byte_array));
		uint8_t first_byte = byte_array.front();
		ByteArraySize byte_array_size;
		if(first_byte < 253){
			byte_array_size = static_cast<ByteArraySize>(first_byte);
			this->header_size = byte_array_size;
		}
		else if(first_byte == 253){
			byte_array_size = static_cast<ByteArraySize>(
				(static_cast<uint16_t>(byte_array[1]) << 8*1) | 
				(static_cast<uint16_t>(byte_array[2]) << 8*0));
			this->header_size = 1+16/8;

		}
		else if(first_byte == 254){
			byte_array_size = static_cast<ByteArraySize>(
				(static_cast<uint32_t>(byte_array[1]) << 8*3) | 
				(static_cast<uint32_t>(byte_array[2]) << 8*2) |
				(static_cast<uint32_t>(byte_array[3]) << 8*1) |
				(static_cast<uint32_t>(byte_array[4]) << 8*0));
			this->header_size = 1+32/8;
		}
		else if(first_byte == 255){
			byte_array_size = static_cast<ByteArraySize>(
				(static_cast<uint64_t>(byte_array[1]) << 8*7) | 
				(static_cast<uint64_t>(byte_array[2]) << 8*6) | 
				(static_cast<uint64_t>(byte_array[3]) << 8*5) |
				(static_cast<uint64_t>(byte_array[4]) << 8*4) |
				(static_cast<uint64_t>(byte_array[5]) << 8*3) | 
				(static_cast<uint64_t>(byte_array[6]) << 8*2) |
				(static_cast<uint64_t>(byte_array[7]) << 8*1) |
				(static_cast<uint64_t>(byte_array[8]) << 8*0));
			this->header_size = 1+64/8;
		}
		auto command = SocketSessionHeader(byte_array_size);
		return command;	
	}
	
	auto Serialize() const -> neuria::ByteArray {
		ByteArray byte_array;
		if(this->byte_array_size < 253){
			byte_array.push_back(this->byte_array_size);
		}
		else if(this->byte_array_size <= pow(2, 16)){
			byte_array.push_back(253);
			uint16_t size16 = static_cast<uint16_t>(this->byte_array_size);
			byte_array.push_back(size16 >> 8);
			byte_array.push_back(size16);
		}
		else if(this->byte_array_size <= pow(2, 32)){
			byte_array.push_back(254);
			uint32_t size32 = static_cast<uint32_t>(this->byte_array_size);
			byte_array.push_back(size32 >> 24);
			byte_array.push_back(size32 >> 16);
			byte_array.push_back(size32 >> 8);
			byte_array.push_back(size32);
		}
		else if(this->byte_array_size <= pow(2, 64)){
			byte_array.push_back(255);
			uint64_t size64 = static_cast<uint64_t>(this->byte_array_size);
			byte_array.push_back(size64 >> 56);
			byte_array.push_back(size64 >> 48);
			byte_array.push_back(size64 >> 40);
			byte_array.push_back(size64 >> 32);
			byte_array.push_back(size64 >> 24);
			byte_array.push_back(size64 >> 16);
			byte_array.push_back(size64 >> 8);
			byte_array.push_back(size64);
		}
		else {
			assert(!"too much bytes!");
		}
		return byte_array;
	}

	auto GetByteArraySize() const -> ByteArraySize { return byte_array_size; }
	auto GetHeaderSize() const -> ByteArraySize { return this->header_size; }

private:
	ByteArraySize byte_array_size;
	ByteArraySize header_size;
};

auto operator<<(std::ostream& os, const SocketSessionHeader& header) -> std::ostream& {
	os << "ByteArraySize : " << header.GetByteArraySize();
	return os;
}

auto operator<<(std::ostream& os, const ByteArray& byte_array) -> std::ostream& {
	for(uint8_t byte : byte_array){
		os << static_cast<int>(byte) << "\t";
	}
	return os;
}

}
}

