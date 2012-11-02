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
    SocketSessionHeader(ByteArraySize body_size) : body_size(body_size) {
		if(body_size < 253){
			header_size = 1;
		}
		else if(body_size < pow(2, 16)){
			header_size = 1 + 16/8;	
		}
		else if(body_size < pow(2, 32)){
			header_size = 1 + 32/8;	
		}
		else if(body_size < pow(2, 64)){
			header_size = 1 + 64/8;	
		}
		else {
			assert(!"too big data!");	
		}
	}

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
		ByteArraySize body_size;
		if(first_byte < 253){
			body_size = static_cast<ByteArraySize>(first_byte);
		}
		else if(first_byte == 253){
			body_size = static_cast<ByteArraySize>(
				(static_cast<uint16_t>(byte_array[1]) << 8*1) | 
				(static_cast<uint16_t>(byte_array[2]) << 8*0));

		}
		else if(first_byte == 254){
			body_size = static_cast<ByteArraySize>(
				(static_cast<uint32_t>(byte_array[1]) << 8*3) | 
				(static_cast<uint32_t>(byte_array[2]) << 8*2) |
				(static_cast<uint32_t>(byte_array[3]) << 8*1) |
				(static_cast<uint32_t>(byte_array[4]) << 8*0));
		}
		else if(first_byte == 255){
			body_size = static_cast<ByteArraySize>(
				(static_cast<uint64_t>(byte_array[1]) << 8*7) | 
				(static_cast<uint64_t>(byte_array[2]) << 8*6) | 
				(static_cast<uint64_t>(byte_array[3]) << 8*5) |
				(static_cast<uint64_t>(byte_array[4]) << 8*4) |
				(static_cast<uint64_t>(byte_array[5]) << 8*3) | 
				(static_cast<uint64_t>(byte_array[6]) << 8*2) |
				(static_cast<uint64_t>(byte_array[7]) << 8*1) |
				(static_cast<uint64_t>(byte_array[8]) << 8*0));
		}
		auto command = SocketSessionHeader(body_size);
		return command;	
	}
	
	auto Serialize() const -> neuria::ByteArray {
		ByteArray byte_array;
		if(this->body_size < 253){
			byte_array.push_back(this->body_size);
		}
		else if(this->body_size <= pow(2, 16)){
			byte_array.push_back(253);
			uint16_t size16 = static_cast<uint16_t>(this->body_size);
			byte_array.push_back(size16 >> 8);
			byte_array.push_back(size16);
		}
		else if(this->body_size <= pow(2, 32)){
			byte_array.push_back(254);
			uint32_t size32 = static_cast<uint32_t>(this->body_size);
			byte_array.push_back(size32 >> 24);
			byte_array.push_back(size32 >> 16);
			byte_array.push_back(size32 >> 8);
			byte_array.push_back(size32);
		}
		else if(this->body_size <= pow(2, 64)){
			byte_array.push_back(255);
			uint64_t size64 = static_cast<uint64_t>(this->body_size);
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

	auto GetBodySize() const -> ByteArraySize { return body_size; }
	auto GetHeaderSize() const -> ByteArraySize { return this->header_size; }

private:
	ByteArraySize header_size, body_size;
};

inline auto operator<<(std::ostream& os, const SocketSessionHeader& header) -> std::ostream& {
	os << "HeaderSize : " << header.GetHeaderSize() << "\nBodySize : " << header.GetBodySize();
	return os;
}

inline auto operator<<(std::ostream& os, const ByteArray& byte_array) -> std::ostream& {
	for(uint8_t byte : byte_array){
		os << static_cast<int>(byte) << "\t";
	}
	return os;
}

}
}

