#include "processor.h"
#include "config.h"
#include <cmath>

PROCESSOR::PROCESSOR(sc_module_name nm, int adderss_)
    :sc_module(nm)
{
    address = adderss_;
    SC_CTHREAD(process, clk_i.pos());
    SC_METHOD(add_to_queue);
    sensitive << clk_i.pos();
}

PROCESSOR::~PROCESSOR()
{
}

template <class T>
T pop_front(std::vector<T>& v) {
    if (v.empty()) throw std::out_of_range("pop_front on empty vector");
    T x = std::move(v.front());
    v.erase(v.begin());
    return x;
}

inline float sigmoid(float x) {
    if (x >= 0.0f) {
        float z = std::exp(-x);
        return 1.0f / (1.0f + z);
    } else {
        float z = std::exp(x);
        return z / (1.0f + z);
    }
}

void PROCESSOR::process() {
    while(true) {
        while(queue_nn_number.size() == 0) {
            wait();
        }
        is_working = true;
        float sum_reg = 0;
        int number_of_nn = pop_front(queue_nn_number);
        for (int i = 0; i < prevmem_size_i; i++) {
            weight_addr_o = contr_start_weight_address_i 
                + (prevmem_size_i + 1) * number_of_nn + i;
            prevmem_addr_o = i;
            #ifdef PROCESSOR_DEBUG
            std::cout << "["<< sc_time_stamp() << "] [PROCESSOR] ["
                << address <<"] TRY GET WEIGHT ADDR: " 
                << weight_addr_o << " DATA ADDR: "
                << prevmem_addr_o << std::endl;
            #endif
            wait(); wait();
            #ifdef PROCESSOR_DEBUG
            std::cout << "["<< sc_time_stamp() << "] [PROCESSOR] ["
                << address <<"] WEIGHT: " 
                << weight_data_i << " DATA: "
                << prevmem_data_i << std::endl;
            #endif
            sum_reg += prevmem_data_i * weight_data_i;
        } 
        weight_addr_o = contr_start_weight_address_i 
                + (prevmem_size_i + 1) * number_of_nn + prevmem_size_i;
        wait(); wait();
        #ifdef PROCESSOR_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PROCESSOR] ["
            << address <<"] WEIGHT: " 
            << weight_data_i << std::endl;
        #endif
        sum_reg += weight_data_i;
        resmem_addr_o = number_of_nn;
        resmem_data_o = sigmoid(sum_reg);
        resmem_w_o = true;
        wait();
        resmem_w_o = false;
        is_working = false;
        #ifdef PROCESSOR_STAT
        count_neurons_calc += 1;
        #endif
    }
}

void PROCESSOR::add_to_queue() {
    if (queue_address_i == address && queue_w_i 
        && queue_nn_number.size() < QUEUE_SIZE) {
        queue_nn_number.push_back(queue_data_i);
        #ifdef PROCESSOR_INPUT_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PROCESSOR] ["
            <<address<<"] ADD TO QUEUE: " 
            << queue_data_i << " QUEUE SIZE: "
            << queue_nn_number.size() << std::endl;
        #endif
        contr_empty_o = false;
    }
    queue_size_o = queue_nn_number.size();
    if (queue_nn_number.size() == 0) {
        contr_empty_o = !is_working;
    }
}
