#include "daudasynthinn_ui.h"
#include "notes.h"

// OPERATOR UI


int OperatorUI::previous_time_ms;

void OperatorUI::update_time(float time_ms){
    OperatorUI::previous_time_ms = time_ms;
}

void OperatorUI::draw_operator(OperatorValue *oper, float ratio, bool editing){
    float *oper_vertices = new float[POINTS_PER_WAVE_GRAPHIC * (int)ratio * 2];

    int important_time;
    if(oper->sync_to_base_frequency){
        important_time = previous_time_ms % (int)(0.5 + 1.0 / 0.0004);
    }
    else{
        important_time = previous_time_ms % (int)(0.5 + 1.0 / (0.0004 * oper->frequency_factor));
    }

    double current_phase = oper->phase_offset + 0.0004 * (double)important_time * 6.28318 * oper->frequency_factor;

    for(int i = 0; i < POINTS_PER_WAVE_GRAPHIC * (int)ratio; i++){
        oper_vertices[i * 2] = (float)i / (float)POINTS_PER_WAVE_GRAPHIC - 0.5 * ratio;
        oper_vertices[i*2+1] = 0.2 * oper->amplitude * sin(current_phase);
        if(!editing){
            if(oper_vertices[i*2+1] > 0.5){
                oper_vertices[i*2+1] = 0.5;
            }
            if(oper_vertices[i*2+1] < -0.5){
                oper_vertices[i*2+1] = -0.5;
            }
        }
        current_phase += 6.28318 * oper->frequency_factor / (double)POINTS_PER_WAVE_GRAPHIC;

        if(oper->sync_to_base_frequency){
            if(((current_phase - oper->phase_offset) / oper->frequency_factor) > 6.28318){
                current_phase = oper->phase_offset + 0.0004 * 6.28318 * oper->frequency_factor;
            }
        }
    }
    draw_line_strip(oper_vertices, POINTS_PER_WAVE_GRAPHIC * (int)ratio, 1.0);
    delete[] oper_vertices;
}

void OperatorUI::draw_modulated_operator(OperatorValue *oper, int oper_count, float ratio, bool editing){
    float *oper_vertices = new float[POINTS_PER_WAVE_GRAPHIC * (int)ratio * 2];

    // float base_phase = (float)previous_time_ms * 0.0004 * 6.28318;
    // float base_phase_incr = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318;

    float *mod_phase = new float[oper_count];
    float *mod_phase_incr = new float[oper_count];
    for(int j = 0; j < oper_count; j++){
        mod_phase[j] = (float)previous_time_ms * 0.0004 * 6.28318 * oper[j].frequency_factor;
        mod_phase_incr[j] = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318 * oper[j].frequency_factor;
    }

    for(int i = 0; i < POINTS_PER_WAVE_GRAPHIC * (int)ratio; i++){
        // base_phase += base_phase_incr;
        double wave_value = 0.0;

        for(int j = oper_count - 1; j >= 0; j--){
            mod_phase[j] += mod_phase_incr[j];
            wave_value = oper[j].amplitude * sin(oper[j].phase_offset + mod_phase[j] + wave_value);
        }
        oper_vertices[i * 2] = (float)i / (float)POINTS_PER_WAVE_GRAPHIC - 0.5 * ratio;
        oper_vertices[i*2+1] = 0.2 * wave_value; //sin(base_phase + wave_value);
        if(!editing){
            if(oper_vertices[i*2+1] > 0.5){
                oper_vertices[i*2+1] = 0.5;
            }
            if(oper_vertices[i*2+1] < -0.5){
                oper_vertices[i*2+1] = -0.5;
            }
        }
    }

    delete[] mod_phase;
    delete[] mod_phase_incr;

    draw_line_strip(oper_vertices, POINTS_PER_WAVE_GRAPHIC * (int)ratio, 1.0);
    delete[] oper_vertices;
}

double get_wave_value_for_operator(OperatorValue *oper, int index, float *mod_phase){
    if(Patch::get_instance().operators_visited_by_graphics[index] == 2){
        return 0.0;
    }
    Patch::get_instance().operators_visited_by_graphics[index] += 1;
    double wave_value = 0.0;
    for(int i = 0; i < Patch::get_instance().modulation_order[index].size(); i++){
        wave_value += get_wave_value_for_operator(oper, Patch::get_instance().modulation_order[index][i], mod_phase);
    }
    Patch::get_instance().operators_visited_by_graphics[index] -= 1;
    return oper[index].amplitude * sin(oper[index].phase_offset + mod_phase[index] + wave_value);
}

void OperatorUI::draw_operator_with_modulation(OperatorValue *oper, int index, float ratio, bool editing){
    float *oper_vertices = new float[POINTS_PER_WAVE_GRAPHIC * (int)ratio * 2];

    // float base_phase = (float)previous_time_ms * 0.0004 * 6.28318;
    // float base_phase_incr = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318;

    int oper_count = Patch::get_instance().modulation_order.size();
    float *mod_phase = new float[oper_count];
    float *mod_phase_incr = new float[oper_count];
    for(int j = 0; j < oper_count; j++){
        int important_time;
        if(oper[j].sync_to_base_frequency){
            important_time = previous_time_ms % (int)(0.5 + 1.0 / 0.0004);
        }
        else{
            important_time = previous_time_ms % (int)(0.5 + 1.0 / (0.0004 * oper[j].frequency_factor));
        }
        mod_phase[j] = (float)important_time * 0.0004 * 6.28318 * oper[j].frequency_factor;
        mod_phase_incr[j] = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318 * oper[j].frequency_factor;
    }

    for(int i = 0; i < POINTS_PER_WAVE_GRAPHIC * (int)ratio; i++){
        // base_phase += base_phase_incr;
        double wave_value = 0.0;

        for(int j = 0; j < oper_count; j++){
            mod_phase[j] += mod_phase_incr[j];
            if(oper[j].sync_to_base_frequency){
                if(((mod_phase[j] - oper[j].phase_offset) / oper[j].frequency_factor) > 6.28318){
                    mod_phase[j] = oper[j].phase_offset + 0.0004 * 6.28318 * oper[j].frequency_factor;
                }
            }
        }

        wave_value = get_wave_value_for_operator(oper, index, mod_phase);

        oper_vertices[i * 2] = (float)i / (float)POINTS_PER_WAVE_GRAPHIC - 0.5 * ratio;
        oper_vertices[i*2+1] = 0.2 * wave_value; //sin(base_phase + wave_value);
        if(!editing){
            if(oper_vertices[i*2+1] > 0.5){
                oper_vertices[i*2+1] = 0.5;
            }
            if(oper_vertices[i*2+1] < -0.5){
                oper_vertices[i*2+1] = -0.5;
            }
        }
    }

    delete[] mod_phase;
    delete[] mod_phase_incr;

    draw_line_strip(oper_vertices, POINTS_PER_WAVE_GRAPHIC * (int)ratio, 1.0);
    delete[] oper_vertices;
}

void OperatorUI::draw_whole_patch(OperatorValue *oper, float ratio, bool editing){
    float *oper_vertices = new float[POINTS_PER_WAVE_GRAPHIC * (int)ratio * 2];

    // float base_phase = (float)previous_time_ms * 0.0004 * 6.28318;
    // float base_phase_incr = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318;

    int oper_count = Patch::get_instance().modulation_order.size();
    float *mod_phase = new float[oper_count];
    float *mod_phase_incr = new float[oper_count];
    for(int j = 0; j < oper_count; j++){
        int important_time;
        if(oper[j].sync_to_base_frequency){
            important_time = previous_time_ms % (int)(0.5 + 1.0 / 0.0004);
        }
        else{
            important_time = previous_time_ms % (int)(0.5 + 1.0 / (0.0004 * oper[j].frequency_factor));
        }
        mod_phase[j] = (float)important_time * 0.0004 * 6.28318 * oper[j].frequency_factor;
        mod_phase_incr[j] = (1.0 / (float)POINTS_PER_WAVE_GRAPHIC) * 6.28318 * oper[j].frequency_factor;
    }

    for(int i = 0; i < POINTS_PER_WAVE_GRAPHIC * (int)ratio; i++){
        // base_phase += base_phase_incr;
        double wave_value = 0.0;

        for(int j = 0; j < oper_count; j++){
            mod_phase[j] += mod_phase_incr[j];
            if(oper[j].sync_to_base_frequency){
                if(((mod_phase[j] - oper[j].phase_offset) / oper[j].frequency_factor) > 6.28318){
                    mod_phase[j] = oper[j].phase_offset + 0.0004 * 6.28318 * oper[j].frequency_factor;
                }
            }
        }

        for(int carrier = 0; carrier < Patch::get_instance().carriers.size(); carrier++){
            wave_value += get_wave_value_for_operator(oper, Patch::get_instance().carriers[carrier], mod_phase);
        }

        oper_vertices[i * 2] = (float)i / (float)POINTS_PER_WAVE_GRAPHIC - 0.5 * ratio;
        oper_vertices[i*2+1] = 0.2 * wave_value; //sin(base_phase + wave_value);
        if(!editing){
            if(oper_vertices[i*2+1] > 0.5){
                oper_vertices[i*2+1] = 0.5;
            }
            if(oper_vertices[i*2+1] < -0.5){
                oper_vertices[i*2+1] = -0.5;
            }
        }
    }

    delete[] mod_phase;
    delete[] mod_phase_incr;

    draw_line_strip(oper_vertices, POINTS_PER_WAVE_GRAPHIC * (int)ratio, 1.0);
    delete[] oper_vertices;
}



// ENVELOPE UI

EnvelopeUI::EnvelopeUI(Envelope<float> *amp_envelope, Envelope<Algorithm> *algo_envelope){

    this->amp_envelope = amp_envelope;
    this->algo_envelope = algo_envelope;
    for(int i = 0; i < algo_envelope->env_values.size(); i++){
        selected_algo_env_points.push_back(true);
    }
}

void EnvelopeUI::init_envelopes(int graphics_start_x, int graphics_top_y, int graphics_width, float envelope_length_ms){

    this->envelope_length_ms = envelope_length_ms;

    dragging_env_index = -1;

    amp_envelope_frame = Area(graphics_start_x + graphics_width / 2, graphics_top_y - 90, graphics_width, 100, 1.0);
    amp_envelope_area = Area(graphics_start_x + graphics_width / 2, graphics_top_y - 90, graphics_width - 4, 96, 0.2);

    algorithm_envelope_frame = Area(graphics_start_x + graphics_width / 2, graphics_top_y - 20, graphics_width, 40, 0.2);
    algorithm_envelope_area = Area(graphics_start_x + graphics_width / 2, graphics_top_y - 20, graphics_width - 4, 36, 0.1);

    add_to_amp_envelope = new Area(amp_envelope_frame.center_x - (amp_envelope_frame.width / 2 + 20),
                            amp_envelope_frame.center_y, 20, 20, 0.4);

    add_to_algo_envelope = new Area(algorithm_envelope_frame.center_x - (algorithm_envelope_frame.width / 2 + 20),
                            algorithm_envelope_frame.center_y, 20, 20, 0.0);


    time_slider = Area(amp_envelope_frame.center_x + amp_envelope_frame.width / 2 + 20,
                            amp_envelope_frame.center_y + amp_envelope_frame.height / 2 + 10, 40, 60, 0.2);

}

void EnvelopeUI::init_graphical_objects(){

    areas.push_back(&amp_envelope_frame);
    areas.push_back(&amp_envelope_area);
    //areas.push_back(&algorithm_envelope_frame);
    //areas.push_back(&algorithm_envelope_area);

    areas.push_back(&time_slider);

}

bool EnvelopeUI::cursor_activate(int cursor_id, int x, int y, Algorithm &algorithm_editor, bool deactivate){

    if(deactivate){ // BUTTON RELEASED
        if(cursor_id == amp_env_cursor_id){
            amp_env_cursor_id = NO_CURSOR_ASSIGNED;
            dragging_env_index = -1;
        }
        else if(cursor_id == algorithm_env_cursor_id){
            algorithm_env_cursor_id = NO_CURSOR_ASSIGNED;
            dragging_env_index = -1;
        }
        else if(cursor_id == time_slider_cursor_id){
            time_slider_cursor_id = NO_CURSOR_ASSIGNED;
        }
    }
    else{ // BUTTON PRESSED

        // selecting env_points
        float trans_x = x - (algorithm_envelope_frame.center_x - algorithm_envelope_frame.width / 2);
        float trans_y = y - (algorithm_envelope_frame.center_y - algorithm_envelope_frame.height / 2);
        for(int i = 0; i < algo_envelope->env_values.size(); i++){
            EnvelopeValue<Algorithm> *env_val = algo_envelope->env_values[i];
            if(Area(algorithm_envelope_frame.width * env_val->time_ms / envelope_length_ms,
                        algorithm_envelope_frame.height, 20, 20, 0.4).is_inside(trans_x, trans_y)){
                selected_algo_env_points[i] = !selected_algo_env_points[i];
                /*if(selected_algo_env_points[i]){
                    algorithm_env_cursor_id = cursor_id;
                    dragging_env_index = i;
                    active_algorithm_env_value_index = i;
                    if(!Patch::get_instance().is_using_algo_envelope()){
                        Patch::get_instance().set_fixed_algo_value(&algo_envelope->env_values[i]->value);
                    }
                    return true;
                }*/
                break;
            }
        }

        if(amp_env_cursor_id == NO_CURSOR_ASSIGNED && amp_envelope_frame.is_inside(x, y)){
            amp_env_cursor_id = cursor_id;
        }
        else if(algorithm_env_cursor_id == NO_CURSOR_ASSIGNED && algorithm_envelope_frame.is_inside(x, y)){
            algorithm_env_cursor_id = cursor_id;
        }
        else if (add_to_amp_envelope->is_inside(x, y)){
            add_amp_env_point(envelope_length_ms / 4, 1.0);
        }
        else if (add_to_algo_envelope->is_inside(x, y)){
            active_algorithm_env_value_index = add_algorithm_env_point(envelope_length_ms / 4, &algorithm_editor);
            return true; // this should now be the active envelope (release or attack)
        }
    }
    return false;
}

bool EnvelopeUI::check_cursors(int cursor_id, int x, int y){

    if(cursor_id == amp_env_cursor_id){
        float trans_x = x - (amp_envelope_frame.center_x - amp_envelope_frame.width / 2);
        float trans_y = y - (amp_envelope_frame.center_y - amp_envelope_frame.height / 2);
        
        if(dragging_env_index == -1){
            for(int i = 0; i < amp_envelope->env_values.size(); i++){
                EnvelopeValue<float> *env_val = amp_envelope->env_values[i];
                if(Area(amp_envelope_frame.width * env_val->time_ms / envelope_length_ms,
                            amp_envelope_frame.height * env_val->value, 20, 20, 0.4).is_inside(trans_x, trans_y)){
                    dragging_env_index = i;
                }
            }
        }
        else{
            update_amp_env_point(envelope_length_ms * trans_x / amp_envelope_frame.width, trans_y / amp_envelope_frame.height,
                        dragging_env_index);
        }
    }
    else if(cursor_id == algorithm_env_cursor_id){
        float trans_x = x - (algorithm_envelope_frame.center_x - algorithm_envelope_frame.width / 2);
        float trans_y = y - (algorithm_envelope_frame.center_y - algorithm_envelope_frame.height / 2);
        
        if(dragging_env_index == -1){
            for(int i = 0; i < algo_envelope->env_values.size(); i++){
                EnvelopeValue<Algorithm> *env_val = algo_envelope->env_values[i];
                if(Area(algorithm_envelope_frame.width * env_val->time_ms / envelope_length_ms,
                            algorithm_envelope_frame.height * 0.25, 20, 20, 0.4).is_inside(trans_x, trans_y)){
                    // selected_algo_env_points[i] = true;
                    dragging_env_index = i;
                    active_algorithm_env_value_index = i;
                    if(!Patch::get_instance().is_using_algo_envelope()){
                        Patch::get_instance().set_fixed_algo_value(&algo_envelope->env_values[i]->value);
                    }
                    return true;
                }
            }
        }
        else{
            update_algorithm_env_point(envelope_length_ms * trans_x / amp_envelope_frame.width, trans_y / amp_envelope_frame.height,
                        dragging_env_index);
        }
    }
    else if(cursor_id == time_slider_cursor_id){
        update_envelope_times((1.0 + (float)(x - time_slider_cursor.getX()) / 100.0));
        time_slider_cursor.setX((float)x);
        time_slider_cursor.setY((float)y);
    }
    else if(time_slider_cursor_id == NO_CURSOR_ASSIGNED && time_slider.is_inside(x, y)){
        time_slider_cursor_id = cursor_id;
        time_slider_cursor.setX((float)x);
        time_slider_cursor.setY((float)y);
    }
    return false;
}

void EnvelopeUI::update_ui(float current_time_ms, float active_note_envelope_time){
    if(active_note_envelope_time > envelope_length_ms){
        moving_line_vertices[2] = moving_line_vertices[0] = amp_envelope_frame.width;
    }
    else{
        moving_line_vertices[2] = moving_line_vertices[0] = amp_envelope_frame.width * active_note_envelope_time / envelope_length_ms;
    }
    if(Patch::get_instance().is_using_algo_envelope()){
        moving_line_vertices[1] = amp_envelope_frame.height + 30;
    }
    else{
        moving_line_vertices[1] = amp_envelope_frame.height;
    }
    if(Patch::get_instance().is_using_amp_envelope()){
        moving_line_vertices[3] = 0.0;
    }
    else{
        moving_line_vertices[3] = amp_envelope_frame.height;
    }
}

void EnvelopeUI::draw_ui(Algorithm &algorithm_editor){
    // STATIC OR NON-DISAPPEARING AREAS
    draw_areas(areas);

    // AMPLITUDE ENVELOPE

    glPushMatrix();
        glTranslatef(amp_envelope_frame.center_x - amp_envelope_frame.width / 2,
                    amp_envelope_frame.center_y - amp_envelope_frame.height / 2, 0.0);
        glPushMatrix();
            glTranslatef(0.0, amp_envelope_frame.height, 0.0);
            glScalef(amp_envelope_frame.width / envelope_length_ms, amp_envelope_frame.height, 1.0);
            draw_ruler(envelope_length_ms);
        glPopMatrix();
        draw_line_strip(moving_line_vertices, 2, 1.0);
        float *env_val_vertices = new float[amp_envelope->env_values.size() * 2];
        for(int i = 0; i < amp_envelope->env_values.size(); i++){
            EnvelopeValue<float> *env_val = amp_envelope->env_values[i];
            draw_rectangle(amp_envelope_frame.width * env_val->time_ms / envelope_length_ms,
                            amp_envelope_frame.height * env_val->value, 20, 20, Color(0.4, 0.4, 0.4));
            env_val_vertices[i * 2] = amp_envelope_frame.width * env_val->time_ms / envelope_length_ms;
            env_val_vertices[i*2+1] = amp_envelope_frame.height * env_val->value;
        }
        draw_line_strip(env_val_vertices, amp_envelope->env_values.size(), 1.0);
        delete[] env_val_vertices;
    glPopMatrix();
    draw_area(*add_to_amp_envelope);

    // ALGORITHM ENVELOPE

    draw_area(*add_to_algo_envelope);
    glPushMatrix();
        glTranslatef(add_to_algo_envelope->center_x, add_to_algo_envelope->center_y, 0.0);
        glScalef(add_to_algo_envelope->width, add_to_algo_envelope->height, 1.0);
        //OperatorUI::draw_modulated_operator(&algorithm_editor.operators[0], algorithm_editor.operators.size(), 1.0, false);
        OperatorUI::draw_whole_patch(&algorithm_editor.operators[0], 1.0, false);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(algorithm_envelope_frame.center_x - algorithm_envelope_frame.width / 2,
                    algorithm_envelope_frame.center_y - algorithm_envelope_frame.height / 2, 0.0);
        draw_line_strip(&moving_line_vertices[4], 2, 1.0);
        int active_env_point = -1;
        for(int i = 0; i < algo_envelope->env_values.size(); i++){
            if(&algo_envelope->env_values[i]->value == &algorithm_editor){
                active_env_point = i;
            }
            else{
                draw_algorithm_envelope_point(i);
            }
        }
        if(active_env_point >= 0){
            draw_algorithm_envelope_point(active_env_point, true);
        }
    glPopMatrix();
}

void EnvelopeUI::draw_algorithm_envelope_point(int index, bool active){
    EnvelopeValue<Algorithm> *env_val = algo_envelope->env_values[index];
    glPushMatrix();
    glTranslatef(algorithm_envelope_frame.width * env_val->time_ms / envelope_length_ms, algorithm_envelope_frame.height, 0.0);
    glScalef(20.0, 20.0, 1.0);
    draw_rectangle(0.0, -1.2, 0.1, 1.0, Color(0.2, 0.2, 0.2));
    draw_rectangle(0.0, -1.2, 0.4, 0.4, Color(0.4, 0.4, 0.4));
    if(active){
        draw_rectangle(0.0, 0.0, 1.2, 1.2, Color(0.7, 0.7, 0.3));
    }
    else if(selected_algo_env_points[index]){
        draw_rectangle(0.0, 0.0, 1.2, 1.2, Color(0.1, 0.1, 0.9));
    }
    draw_rectangle(0.0, 0.0, 1.0, 1.0, Color(0.2, 0.2, 0.2));
    //OperatorUI::draw_modulated_operator(&algo_envelope->env_values[index]->value.operators[0], algo_envelope->env_values[index]->value.operators.size(), 1.0, false);
    OperatorUI::draw_whole_patch(&algo_envelope->env_values[index]->value.operators[0], 1.0, false);
    glPopMatrix();
}

Algorithm &EnvelopeUI::get_active_envelope_value(){
    return algo_envelope->env_values[active_algorithm_env_value_index]->value;
}

void EnvelopeUI::update_operator_for_all_selected(int index, float frequency_factor_factor, float amplitude_factor, float phase_offset_offset, bool active){

    for(int i = 0; i < selected_algo_env_points.size(); i++){
        Algorithm &algorithm_editor = algo_envelope->env_values[i]->value;

        if(selected_algo_env_points[i] || (active && i == active_algorithm_env_value_index)){
            algorithm_editor.operators[index].frequency_factor *= frequency_factor_factor;
            algorithm_editor.operators[index].phase_offset += phase_offset_offset;

            if(algorithm_editor.operators[index].amplitude < 0.1){
                algorithm_editor.operators[index].amplitude = 0.1;
            }
            algorithm_editor.operators[index].amplitude *= amplitude_factor;
            if(algorithm_editor.operators[index].amplitude <= 0.1){
                algorithm_editor.operators[index].amplitude = 0.0;
            }
        }
    }
}


void EnvelopeUI::add_amp_env_point(float time_ms, float value){
    amp_envelope->insert_envelope_value(new EnvelopeValue<float>(value, time_ms));
}

void EnvelopeUI::update_amp_env_point(float time_ms, float value, int index){
    //Envelope<float> &amp_env = Patch::get_instance().amp_attack_envelope;
    EnvelopeValue<float> *env_val = amp_envelope->env_values[index];

    if(value < 0.0){
        value = 0.0;
    }
    if(time_ms < 0){
        time_ms = 0;
    }
    else if(time_ms > envelope_length_ms || (index >= amp_envelope->env_values.size() - 1 && time_ms < envelope_length_ms)){
        envelope_length_ms += 0.1 * (time_ms - envelope_length_ms);  // ACTUALLY CHANGE THE ENVELOPE LENGTH
        time_ms = envelope_length_ms;
        // MAKE SURE ALGORITHM ENVELOPE VALUES DON'T GO OFF THE SCREEN
        for(int i = algo_envelope->env_values.size() - 1; i >= 0 && algo_envelope->env_values[i]->time_ms > envelope_length_ms; i--){
            algo_envelope->env_values[i]->time_ms = envelope_length_ms;
        }
    }
    env_val->value = value;
    // if(index < 1){
    //     return;
    // }
    env_val->time_ms = time_ms;
    int new_index = amp_envelope->insert_envelope_value(env_val);
    if(new_index != index && new_index >= 0){
        dragging_env_index = new_index;
    }
    else{
        // JUST TO MAKE SURE IF insert RETURNS -1
        new_index = index;
    }
}

void EnvelopeUI::update_envelope_times(float scaling_factor){
    std::vector<EnvelopeValue<float> *> amp_env_values = amp_envelope->env_values;
    for(int i = 0; i < amp_env_values.size(); i++){
        amp_env_values[i]->time_ms *= scaling_factor;
    }

    std::vector<EnvelopeValue<Algorithm> *> algo_env_values = algo_envelope->env_values;
    for(int i = 0; i < algo_env_values.size(); i++){
        algo_env_values[i]->time_ms *= scaling_factor;
    }

    envelope_length_ms *= scaling_factor;
}

int EnvelopeUI::add_algorithm_env_point(float time_ms, Algorithm *added_value){
    int added_index;
    if(added_value == NULL){
        added_index = algo_envelope->insert_envelope_value(new EnvelopeValue<Algorithm>(
                        algo_envelope->env_values[active_algorithm_env_value_index]->value, time_ms));
    }
    else{
        added_index = algo_envelope->insert_envelope_value(new EnvelopeValue<Algorithm>(*added_value, time_ms));
    }
    selected_algo_env_points.insert(selected_algo_env_points.begin() + added_index, true);
    return added_index;
}

Algorithm EnvelopeUI::update_algorithm_env_point(float time_ms, float y_coord, int index){
    // Envelope<Algorithm> &algo_env = Patch::get_instance().algo_attack_envelope;
    if(y_coord < 0.0){
        y_coord = 0.0;
    }
    else if(y_coord > 1.0){
        y_coord = 1.0;
    }
    if(time_ms < 0){
        time_ms = 0;
    }
    else if(time_ms > envelope_length_ms){
        time_ms = envelope_length_ms;
    }
    EnvelopeValue<Algorithm> *env_val = algo_envelope->env_values[index];
    env_val->time_ms = time_ms;
    //env_val->value = NO CHANGE NEEDED, ONLY CHANGING THE TIME HERE
    int new_index = algo_envelope->insert_envelope_value(env_val);
    if(new_index != index && new_index >= 0){
        // printf("new_index: %d, index: %d\n", new_index, index);
        // printf("CHANGE!!!\n");

        active_algorithm_env_value_index = dragging_env_index = new_index;
        bool bs = selected_algo_env_points[index];
        selected_algo_env_points[index] = selected_algo_env_points[new_index];
        selected_algo_env_points[new_index] = bs;
    }
    else{
        // JUST TO MAKE SURE IF insert RETURNS -1
        new_index = index;
    }

    return env_val->value;
}

void EnvelopeUI::remove_active_algorithm_env_point(){
    if(algo_envelope->remove_envelope_value(active_algorithm_env_value_index)){
        selected_algo_env_points.erase(selected_algo_env_points.begin() + active_algorithm_env_value_index);
        if(active_algorithm_env_value_index >= algo_envelope->env_values.size()){
            --active_algorithm_env_value_index;
        }
    }
}

void EnvelopeUI::select_all_algorithm_env_points(bool select_all){
    for(int i = 0; i < selected_algo_env_points.size(); i++){
        selected_algo_env_points[i] = select_all;
    }
}