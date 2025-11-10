#ifndef _IO_H
#define _IO_H

#include "systemc.h"
#include <string>
extern std::string g_configFile;
extern std::string g_pictureFile;
extern std::string g_outputFile;

SC_MODULE(IO)
{
    sc_in<bool>     clk_i;
    sc_out<int>     addr_o;
    sc_inout<float> data_io;
    sc_inout<bool>  wr_io;
    
    SC_HAS_PROCESS(IO);
    
    IO(sc_module_name nm);
    ~IO();
    
    // Задача - передача данных в систему и запуск вычислений
    void write_data_to_system();
    // Задача - чтение итоговых данных по готовности системы
    void read_data_from_system();
    
private: 
    int counter = 0;
    bool transmit = false;
};


#endif
