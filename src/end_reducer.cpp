#include "end_reducer.h"

END_REDUCER::END_REDUCER(sc_module_name nm)
: sc_module(nm)
, contr_empty_i("contr_empty_i")
{
    contr_empty_i.init(COUNT_PC_UNIT);

    SC_METHOD(reduce);
    sensitive << gen_end_i;
    for (size_t i = 0; i < contr_empty_i.size(); ++i) {
        sensitive << contr_empty_i[i];
    }
}

END_REDUCER::~END_REDUCER() {}

void END_REDUCER::reduce()
{
    bool all_ok = gen_end_i.read();
    for (size_t i = 0; i < contr_empty_i.size(); ++i) {
        all_ok = all_ok && contr_empty_i[i].read();
    }
    cycle_ended_o.write(all_ok);
}
