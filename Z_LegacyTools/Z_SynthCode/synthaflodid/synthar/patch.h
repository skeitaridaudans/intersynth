#ifndef PATCH_H_942629834
#define PATCH_H_942629834

#include "daudasynthinn_music.h"

class Patch{
public:
    static Patch &get_instance();

    void add_operator_to_algorithm(); // ADD VARIABLES LATER FOR SPECIFIC ADDED OPERATORS
    void add_modulation(int modulated, int modulated_by);
    void toggle_modulation(int modulated, int modulated_by);
    bool modulation_exists(int modulated, int modulated_by);
    void add_carrier(int carrier_index);
    void toggle_carrier(int carrier_index);
    bool is_carrier(int carrier_index);

    void toggle_operator_sync(int index);

    float get_amp_value(float time_ms, bool release, float *initial_value = NULL);
    Algorithm get_algo_value(float time_ms, bool release, Algorithm *initial_value = NULL);

    bool past_release(float time_since_release);

    bool toggle_amp_envelope();
    bool toggle_algo_envelope();

    bool is_using_amp_envelope();
    bool is_using_algo_envelope();

    void set_fixed_algo_value(Algorithm *val);

    std::vector<int> carriers;
    std::vector<int> sync_operators;
    std::vector< std::vector<int> > modulation_order;
    std::vector<int> operators_visited_by_graphics;
    std::vector<int> operators_visited_by_sound;

    Envelope<float> amp_attack_envelope;
    Envelope<Algorithm> algo_attack_envelope;

    Envelope<float> amp_release_envelope;
    Envelope<Algorithm> algo_release_envelope;

private:
    Patch();

    bool using_amp_envelope;
    bool using_algo_envelope;

    Algorithm *fixed_algorithm;
};

#endif //PATCH_H_942629834