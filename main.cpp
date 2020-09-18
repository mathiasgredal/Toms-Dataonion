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

std::string decodeASCII85Chunk(std::string chunk)
{
    if (chunk.length() != 5)
        throw std::runtime_error("ERROR: incorrect size chunk passed to chunkdecoder");

    int ascii_binary = 0;

    for (size_t i = 0; i < 5; i++) {
        const int base85 = static_cast<int>(chunk[i]) - 33;
        ascii_binary += static_cast<int>((base85 * pow(85, 4 - i)));
    }

    // Extract bits from int using bitshifting
    std::string out;
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
    std::stringstream messagewithpadding;
    for (auto chunk : chunks)
        messagewithpadding << decodeASCII85Chunk(chunk);

    // 4. Remove leftover pad from step 2
    std::string message = messagewithpadding.str();
    if (padding_size != 5)
        message = message.substr(0, message.length() - static_cast<size_t>(padding_size));

    return message;
}

//uint8_t flipEverySecondBit(uint8_t byte)
//{

//}

int main()
{
    // Layer 0
    std::string payload_filedata = readFile("../onionfile.txt");
    std::string layer0 = readPayload(payload_filedata);
    std::string layer0payload = decodeASCII85(layer0);

    // Layer 1
    std::string layer1payload = decodeASCII85(layer0payload);

    //    for (auto byte : layer1payload) {
    //        uint8_t flippedbyte =
    //    }

    // Process 1 byte at a time
    // Flip every second bit using XOR
    // Rotate 1 bit to the right, setting the last bit on the front

    return 0;
}
