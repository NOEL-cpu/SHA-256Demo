#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <iomanip>
#include <thread>
#include <algorithm>    // std::sort
#include "easySha-256.h"

using namespace std;

// Структура для хранения информации о блоке
struct WordBlock {
    std::string current_word;
    std::string word_pair;
    std::string hex_value;
    bool valid;
};

// Функция для обработки блока
void process_block(WordBlock& block) {
    string strBlock = (string)block.word_pair;
   // block.hex_value
    //block.hex_value = hex(block.word_pair);
    vector<unsigned char> hash = sha256(strBlock);  // Получаем SHA-256 хэш пары слов
    block.hex_value = to_hex_string(hash);
    // Проверяем, заканчивается ли хэш на цифры от 0 до 7
    if (block.hex_value.back() >= '0' && block.hex_value.back() <= '7') {
        block.valid = true;
    }
    else {
        block.valid = false;
    }
}

string inputData(){
    ifstream file("input.txt", std::ios::in | std::ios::binary);  // Открытие файла в бинарном режиме
    if (!file.is_open()) {  // Проверка, открылся ли файл
        std::cerr << "Не удалось открыть файл!" << std::endl;
        return "stop";
    }

    std::ostringstream contentStream;
    contentStream << file.rdbuf();  // Чтение всего файла в строковый поток

    std::string content = contentStream.str();  // Преобразование потока в строку

    file.close();  // Закрытие файла

}

 // Функция для разбиения текста на блоки
std::vector<WordBlock> process_text(const std::string & text) {
    std::vector<WordBlock> blocks;
    std::istringstream stream(text);
    std::string previous_word, current_word;
   while (stream >> current_word) {
        if (!previous_word.empty()) {
            WordBlock block;
            block.current_word = current_word;
            block.word_pair = previous_word + " " + current_word;
            block.valid = false;  // Изначально блок считается недействительным
            blocks.push_back(block);
        }
        previous_word = current_word;
    }
    return blocks;
}

int main()
{
    std::cout << "Hello World!\n";
    //считывание содержимого файла в буфер
    string content = inputData();
    cout << content << endl;

    // Разбиваем текст на блоки
    auto blocks = process_text(content);

    // Создаём потоки для обработки блоков
    std::vector<thread> threads;
    for (auto& block : blocks) {
        threads.push_back(thread(process_block, ref(block)));
    }

    // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Записываем промежуточный результат в файл для проверки ликвидности сортировки. Возможно не верное шифрование  Sha 256
    std::ofstream out_file_H("outputWithHex.txt");
    if (out_file_H.is_open()) {
        for (const auto& printBloks : blocks) {
            
            out_file_H << printBloks.current_word << ": " << printBloks.word_pair << ": " << printBloks.hex_value << std::endl;
        }
        out_file_H.close();
    }
    else {
        std::cerr << "Unable to open output file!" << std::endl;
    }

    // Фильтруем блоки, оставляя только те, которые прошли проверку
    std::vector<WordBlock> valid_blocks;
    for (const auto& block : blocks) {
        if (block.valid) {
            valid_blocks.push_back(block);
        }
    }

    // Подсчитываем частоту слов
    unordered_map<std::string, int> word_count;
    for (const auto& block : valid_blocks) {
        word_count[block.current_word]++;
    }

    // Сортируем слова по частоте
    std::vector<std::pair<std::string, int>> sorted_word_count(word_count.begin(), word_count.end());
    std::sort(sorted_word_count.begin(), sorted_word_count.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });


    // Записываем результат в файл
    std::ofstream out_file("output.txt");
    if (out_file.is_open()) {
        for (const auto& pair : sorted_word_count) {
            if (pair.second == 1) continue;
            out_file << pair.first << ": " << pair.second << std::endl;
        }
        out_file.close();
    }
    else {
        std::cerr << "Unable to open output file!" << std::endl;
    }

    return 0;
}




