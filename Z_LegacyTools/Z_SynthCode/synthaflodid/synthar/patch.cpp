#include "patch.h"

// std::vector<int> carriers;
// std::vector< std::vector<int> > modulation_order;
// std::vector<int> operators_visited_by_graphics;
// std::vector<int> operators_visited_by_sound;

Patch &Patch::get_instance(){
    static Patch instance;
    return instance;
}

Patch::Patch(){
    algo_attack_envelope.insert_envelope_value(new EnvelopeValue<Algorithm>());
    algo_release_envelope.insert_envelope_value(new EnvelopeValue<Algorithm>());

    // this->amp_attack_envelope.insert_envelope_value(new EnvelopeValue<float>());

    using_amp_envelope = true;
    using_algo_envelope = true;
    fixed_algorithm = NULL;
}

void Patch::add_operator_to_algorithm(){
    for(int i = 0; i < algo_attack_envelope.env_values.size(); i++){
        algo_attack_envelope.env_values[i]->value.operators.push_back(OperatorValue());
    }
    for(int i = 0; i < algo_release_envelope.env_values.size(); i++){
        algo_release_envelope.env_values[i]->value.operators.push_back(OperatorValue());
    }
    // if(modulation_order.size() == 0){
    //     carriers.push_back(modulation_order.size());
    // }
    // else{
    //     modulation_order[modulation_order.size() - 1].push_back(modulation_order.size());
    // }
    modulation_order.push_back(std::vector<int>());
    sync_operators.push_back(-1);
    operators_visited_by_graphics.push_back(0);
    operators_visited_by_sound.push_back(0);
}

void Patch::add_modulation(int modulated, int modulated_by){
    for(int i = 0; i < modulation_order[modulated].size(); i++){
        if(modulation_order[modulated][i] == modulated_by){
            return;            
        }
    }
    modulation_order[modulated].push_back(modulated_by);
}

void Patch::toggle_modulation(int modulated, int modulated_by){
    for(int i = 0; i < modulation_order[modulated].size(); i++){
        if(modulation_order[modulated][i] == modulated_by){
            modulation_order[modulated].erase(modulation_order[modulated].begin() + i);
            return;            
        }
    }
    modulation_order[modulated].push_back(modulated_by);
}

bool Patch::modulation_exists(int modulated, int modulated_by){
    for(int i = 0; i < modulation_order[modulated].size(); i++){
        if(modulation_order[modulated][i] == modulated_by){
            return true;            
        }
    }
    return false;
}

void Patch::add_carrier(int carrier_index){
    for(int i = 0; i < carriers.size(); i++){
        if(carriers[i] == carrier_index){
            return;            
        }
    }
    carriers.push_back(carrier_index);
}

void Patch::toggle_carrier(int carrier_index){
    for(int i = 0; i < carriers.size(); i++){
        if(carriers[i] == carrier_index){
            carriers.erase(carriers.begin() + i);
            return;
        }
    }
    carriers.push_back(carrier_index);
}

bool Patch::is_carrier(int carrier_index){
    for(int i = 0; i < carriers.size(); i++){
        if(carriers[i] == carrier_index){
            return true;
        }
    }
    return false;
}

void Patch::toggle_operator_sync(int index){
    bool sync_value = (sync_operators[index] < 0);
    if(sync_value){
        sync_operators[index] = 0;
    }
    else{
        sync_operators[index] = -1;
    }
    for(int i = 0; i < algo_attack_envelope.env_values.size(); i++){
        algo_attack_envelope.env_values[i]->value.operators[index].sync_to_base_frequency = sync_value;
    }
    for(int i = 0; i < algo_release_envelope.env_values.size(); i++){
        algo_release_envelope.env_values[i]->value.operators[index].sync_to_base_frequency = sync_value;
    }
}

float Patch::get_amp_value(float time_ms, bool release, float *initial_value){

    if(using_amp_envelope){
        if(release){
            return amp_release_envelope.get_current_value(time_ms, initial_value);
        }
        else{
            return amp_attack_envelope.get_current_value(time_ms, initial_value);
        }
    }
    else{
        if(release){
            return 0.0;
        }
        else{
            return 1.0;
        }
    }
}

Algorithm Patch::get_algo_value(float time_ms, bool release, Algorithm *initial_value){
    
    if(using_algo_envelope || fixed_algorithm == NULL){
        if(release){
                return algo_release_envelope.get_current_value(time_ms, initial_value);
        }
        else{
            return algo_attack_envelope.get_current_value(time_ms, initial_value);
        }
    }
    else{
        return *fixed_algorithm;
    }
}

bool Patch::past_release(float time_since_release){
    return amp_release_envelope.is_finished(time_since_release);
}

bool Patch::toggle_amp_envelope(){
    using_amp_envelope = !using_amp_envelope;
    return using_amp_envelope;
}

bool Patch::toggle_algo_envelope(){
    using_algo_envelope = !using_algo_envelope;
    return using_algo_envelope;
}

bool Patch::is_using_amp_envelope(){
    return using_amp_envelope;
}

bool Patch::is_using_algo_envelope(){
    return using_algo_envelope;
}

void Patch::set_fixed_algo_value(Algorithm *val){
    fixed_algorithm = val;
}

