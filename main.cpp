#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

std::string readFile(std::string filepath)
{
    auto file = std::ifstream(filepath);
    if (!file)
        throw std::runtime_error("Could not open file" + filepath);
    stringstream datastream;
    datastream << file.rdbuf();
    return datastream.str();
}

std::string readPayload(std::string payloadwithmessage)
{
    std::string payload = payloadwithmessage.substr(payloadwithmessage.find("<~"));

    while (payload.find('\n') != -1) {
        payload = payload.erase(payload.find('\n'), 1);
    }

    return payload.substr(payload.find("<~"));
}

std::vector<uint8_t> decodeASCII85Chunk(std::string chunk)
{
    if (chunk.length() != 5) {
        auto up = std::runtime_error("ERROR: incorrect size chunk passed to chunkdecoder");
        throw up;
    }

    int ascii_binary = (chunk[0] - 33) * 85 * 85 * 85 * 85
        + (chunk[1] - 33) * 85 * 85 * 85
        + (chunk[2] - 33) * 85 * 85
        + (chunk[3] - 33) * 85
        + (chunk[4] - 33);

    // Extract bits from int using bitshifting
    std::vector<uint8_t> out;
    out.push_back((ascii_binary << 0) >> 24);
    out.push_back((ascii_binary << 8) >> 24);
    out.push_back((ascii_binary << 16) >> 24);
    out.push_back((ascii_binary << 24) >> 24);

    return out;
}

std::string decodeASCII85(std::string encoded)
{
    // 1. Verify beginning and end with <~ and ~>
    if (encoded.find("<~") != 0)
        throw std::runtime_error("Could not find start indicator");
    if (encoded.find("~>") != encoded.size() - 2)
        throw std::runtime_error("Could not find end indicator");

    std::string encodedRaw = encoded.substr(2, encoded.size() - 4);

    // 2. Split payload into chunks and pad
    int padding_size = 5 - (encodedRaw.size()) % 5;

    if (padding_size != 5) {
        for (int i = 0; i < padding_size; i++)
            encodedRaw.push_back('u');
    }

    std::vector<std::string> chunks = {};

    for (size_t j = 0; j < encodedRaw.size(); j += 5) {
        chunks.push_back(encodedRaw.substr(j, 5));
    }

    // 3. Decode payload 1 chunk at a time
    std::string decodedwithpadding;
    for (auto chunk : chunks) {
        auto decodedChunk = decodeASCII85Chunk(chunk);
        decodedwithpadding.insert(decodedwithpadding.end(), decodedChunk.begin(), decodedChunk.end());
    }

    // 4. Remove leftover pad from step 2
    if (padding_size != 5)
        decodedwithpadding.resize(decodedwithpadding.size() - static_cast<size_t>(padding_size));

    return decodedwithpadding;
}

uint8_t flipEverySecondBit(uint8_t byte)
{
    return byte ^ 0b01010101;
}

uint8_t rotateRight(uint8_t byte)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
    uint8_t firstBit = (byte & 0b00000001) << 7;
#pragma clang diagnostic pop

    byte = byte >> 1;

    return byte | firstBit;
}

// Returns true if parity matches and false if it doesnt
bool calculateParity(uint8_t byte)
{
    int num1s = bitset<8>(byte >> 1).count();

    if (num1s % 2 == 0 && bitset<8>(byte)[0] == 0)
        return true;

    if (num1s % 2 == 1 && bitset<8>(byte)[0] == 1)
        return true;

    return false;
}

int main()
{
    // Layer 0
    std::string layer0 = readFile("../onionfile.txt");
    std::string layer0payload = readPayload(layer0);
    std::string layer1 = decodeASCII85(layer0payload);

    // Layer 1
    std::string layer1payload = decodeASCII85(readPayload(layer1));
    std::string layer2 = "";

    for (auto byte : layer1payload) {
        uint8_t flippedbyte = flipEverySecondBit(byte);
        uint8_t rotatedByte = rotateRight(flippedbyte);
        layer2.push_back(rotatedByte);
    }

    //Layer 2
    std::string layer2payload = decodeASCII85(readPayload(layer2));
    std::string layer3 = "";

    std::vector<uint8_t> temp = {};
    for (auto c : layer2payload) {
        // If byte parity is correct, push onto temp
        if (calculateParity(c))
            temp.push_back(c);

        if (temp.size() == 8) {
            // Remove parity bits and push the 7 characters onto layer3
            // Merge temp into an int64
            uint64_t chunk = 0;
            for (int i = 0; i < 8; i++) {
                chunk = (chunk << 7) | ((uint64_t)temp.at(i) >> 1);
            }

            // Pull out the 7 characters from big int
            std::string outstr = "";
            outstr.push_back((chunk << 56) >> 56);
            outstr.push_back((chunk << 48) >> 56);
            outstr.push_back((chunk << 40) >> 56);
            outstr.push_back((chunk << 32) >> 56);
            outstr.push_back((chunk << 24) >> 56);
            outstr.push_back((chunk << 16) >> 56);
            outstr.push_back((chunk << 8) >> 56);
            outstr.push_back((chunk << 0) >> 56);

            // Reverse because algorithms. Idk it was backwards before it was reversed
            std::reverse(outstr.begin(), outstr.end());

            layer3.insert(layer3.end(), outstr.begin(), outstr.end());

            // Clear string
            temp.clear();
        }
    }

    std::cout << layer3 << std::endl;

    return 0;
}
