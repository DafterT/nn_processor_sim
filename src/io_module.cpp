#include "io_module.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

std::string g_configFile = "input/nn_config.txt";
std::string g_pictureFile = "input/picture.txt";
std::string g_outputFile; 

IO::IO(sc_module_name nm)
    :sc_module(nm)
{
    addr_o.initialize(0);
    data_io.initialize(0);
    wr_io.initialize(false);

    SC_CTHREAD(write_data_to_system, clk_i.pos());

    SC_METHOD(read_data_from_system);
    sensitive << clk_i.pos();
}

IO::~IO()
{
}

bool loadData(
    int& layersCount,
    std::vector<int>& layers,
    std::vector<float>& weights,
    std::vector<float>& picture
) {
    ifstream cfg(g_configFile);
    if (!cfg) return false;

    // 1-3. Читаем число слоёв и сами слои
    cfg >> layersCount;
    layers.resize(layersCount);
    for (int i = 0; i < layersCount; ++i) {
        cfg >> layers[i];
    }

    // 4. Всё остальное из config.txt это веса (float) до конца файла
    float w;
    while (cfg >> w) {
        weights.push_back(w);
    }
    cfg.close();

    // Читаем picture.txt целиком в массив
    ifstream pic(g_pictureFile);
    if (!pic) return false;

    float p;
    while (pic >> p) {
        picture.push_back(p);
    }
    pic.close();

    return true;
}

// Задача - передача данных в систему и запуск вычислений
void IO::write_data_to_system()
{
    // Считывание файла
    transmit = true;
    int layersCount = 0;
    std::vector<int> layers;
    std::vector<float> weights;
    std::vector<float> picture;

    if (!loadData(layersCount, layers, weights, picture)) {
        cout << "[FATAL] Не удалось открыть один из файлов.\n";
        sc_abort();
    }
    // Сигнал о начале передачи
    addr_o.write(0);
    data_io.write(INFINITY);
    wr_io.write(true);
    wait();
    // Передача числа слоев
    data_io.write(layersCount);
    wait();
    // Передача слоев
    addr_o.write(1);
    for (int item: layers) {
        data_io.write(item);
        wait();
    }
    // Передача картинки
    addr_o.write(2);
    for (auto item: picture) {
        data_io.write(item);
        wait();
    }
    // Передача весов
    addr_o.write(3);
    for (float item: weights) {
        data_io.write(item);
        wait();
    }
    // Запуск
    addr_o.write(0);
    data_io.write(-INFINITY);
    wr_io.write(true);
    wait();
    wr_io.write(false);
    transmit = false;
}

// Задача - чтение итоговых данных по готовности системы
void IO::read_data_from_system()
{
    if (!transmit && wr_io) {
        std::ostream* os = &std::cout;
        static std::ofstream fout;
        if (!g_outputFile.empty()) {
            if (!fout.is_open()) {
                fout.open(g_outputFile);
            }
            if (fout) os = &fout;
        }
        (*os) << "["<< sc_time_stamp() << "] [IO] RESULT "<< counter <<": "
              << std::fixed << std::setprecision(9) << data_io << std::endl;
        counter++;
    }
}