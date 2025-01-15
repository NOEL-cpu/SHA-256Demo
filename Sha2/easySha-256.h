#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>


// Константы для SHA-256
const unsigned int K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0x319d2b79, 0x2e9d9c7f, 0x1bbe88db, 0x9b05688c, 0x3c6ef372, 0x7a3d24e8, 0x9b8d5c7b,
    0x5f03b79e, 0x4b1f5f21, 0x17b3ec1e, 0x6d6c0a89, 0xd8f1d2f4, 0x3a5d6e2e, 0x3f8c5f85, 0x7c5c8c5f,
    0x4d4e6f3f, 0x7f0c3a4d, 0x6f3b4fb5, 0x93c9e1e2, 0x8a9e9f5a, 0x6e5c8e6e, 0x5b99bde1, 0x98d3e9e9,
    0x8c1e45a5, 0x7d2f7be1, 0x4e3b2b6b, 0x8f0e2e22, 0x3f8b7d6c, 0x8b7b3a92, 0x4c734b7f, 0x2c7f4d5f,
    0x5f8f2e9e, 0x4e3f9b9f, 0x7b8f0c3e, 0x7f9f9e0a, 0x7c7e4f5f, 0x10d02d2f };


unsigned int H[8] = {
   0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

unsigned int rotr(unsigned int x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

unsigned int ch(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (~x & z);
}

unsigned int maj(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

unsigned int sig0(unsigned int x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

unsigned int sig1(unsigned int x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

unsigned int Sum0(unsigned int x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

unsigned int Sum1(unsigned int x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

std::string to_hex_string(const std::vector<unsigned char>& data) {
    std::stringstream ss;
    for (auto byte : data) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return ss.str();
}

std::vector<unsigned char> sha256(const std::string& input) {
    std::vector<unsigned char> data(input.begin(), input.end());

    unsigned int length = data.size() * 8; // Длина данных в битах

    data.push_back(0x80); // Добавляем бит 1 в конец

        // Дополняем данные до кратности 512 бит (64 байта)
    while ((data.size() * 8) % 512 != 448) {
        data.push_back(0x00);
    }

    // Добавляем длину данных в конец
    for (int i = 7; i >= 0; i--) {
        data.push_back((length >> (i * 8)) & 0xFF);
    }

    // Обработка блоков по 512 бит (64 байта)
    std::vector<unsigned int> w(64);
    unsigned int a, b, c, d, e, f, g, h;
    for (size_t i = 0; i < data.size() / 64; ++i) {
        // Разбиваем блок на 16 слов по 32 бита
        for (int t = 0; t < 16; ++t) {
            w[t] = (data[i * 64 + t * 4] << 24) | (data[i * 64 + t * 4 + 1] << 16) |
                (data[i * 64 + t * 4 + 2] << 8) | data[i * 64 + t * 4 + 3];
        }

        // Дополняем блок до 64 слов
        for (int t = 16; t < 64; ++t) {
            w[t] = sig1(w[t - 2]) + w[t - 7] + sig0(w[t - 15]) + w[t - 16];
        }

        // Инициализация переменных для этого блока
        a = H[0];
        b = H[1];
        c = H[2];
        d = H[3];
        e = H[4];
        f = H[5];
        g = H[6];
        h = H[7];

        // Основной цикл
        for (int t = 0; t < 64; ++t) {
            unsigned int t1 = h + Sum1(e) + ch(e, f, g) + K[t] + w[t];
            unsigned int t2 = Sum0(a) + maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        // Обновляем значения
        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    // Преобразуем итоговые значения в хеш
    std::vector<unsigned char> hash(32);
    for (int i = 0; i < 8; ++i) {
        hash[i * 4] = (H[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (H[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (H[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = H[i] & 0xFF;
    }

    return hash;
}

/*int main() {
    std::string input = "Hello world my dear wallet";
    std::vector<unsigned char> hash = sha256(input);
    std::cout << "SHA-256 hash of \"" << input << "\": " << to_hex_string(hash) << std::endl;
    return 0;
}
*/
