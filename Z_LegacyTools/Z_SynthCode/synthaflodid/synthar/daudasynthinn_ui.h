#ifndef DAUDASYNTHINN_UI_H_82436482734
#define DAUDASYNTHINN_UI_H_82436482734

#include "helpers_graphics.h"
#include "daudasynthinn_music.h"
#include "patch.h"
#include "notes.h"

#include <iostream>
#include <fstream>
#include <map>
#include <math.h>

#define POINTS_PER_WAVE_GRAPHIC 500
#define NO_CURSOR_ASSIGNED -1

static int screen_width = 1200;
static int screen_height = 720;

static int touch_start_x = 200;
static int touch_start_y = 200;


class OperatorUI{
public:
    static void update_time(float time_ms);
    static void draw_operator(OperatorValue *oper, float ratio, bool editing);
    static void draw_modulated_operator(OperatorValue *oper, int oper_count, float ratio, bool editing);
    static void draw_operator_with_modulation(OperatorValue *oper, int index, float ratio, bool editing);
    static void draw_whole_patch(OperatorValue *oper, float ratio, bool editing);
private:
    static int previous_time_ms;
};


class EnvelopeUI{
public:
    EnvelopeUI(){};
    EnvelopeUI(Envelope<float> *amp_envelope, Envelope<Algorithm> *algo_envelope);

    void init_envelopes(int graphics_start_x, int graphics_top_y, int graphics_width, float envelope_length_ms);
    void init_graphical_objects();
    // void cursor_entry(int cursor_id, bool exit = false);
    bool cursor_activate(int cursor_id, int x, int y, Algorithm &algorithm_editor, bool deactivate = false);
    bool check_cursors(int cursor_id, int x, int y);
    void update_ui(float current_time_ms, float active_note_envelope_time);
    void draw_ui(Algorithm &algorithm_editor);
    Algorithm &get_active_envelope_value();
    void update_operator_for_all_selected(int index, float frequency_factor_factor, float amplitude_factor, float phase_offset_offset, bool active);

    void add_amp_env_point(float time_ms, float value);
    int add_algorithm_env_point(float time_ms, Algorithm *added_value = NULL);

    void remove_active_algorithm_env_point();
    void select_all_algorithm_env_points(bool select_all);
private:
    Envelope<float> *amp_envelope;
    Envelope<Algorithm> *algo_envelope;

    int amp_env_cursor_id = NO_CURSOR_ASSIGNED;
    int algorithm_env_cursor_id = NO_CURSOR_ASSIGNED;
    int time_slider_cursor_id = NO_CURSOR_ASSIGNED;

    Cursor time_slider_cursor;

    std::vector<Area *> areas;

    Area amp_envelope_frame;
    Area amp_envelope_area;

    Area algorithm_envelope_frame;
    Area algorithm_envelope_area;

    Area time_slider;

    Area *add_to_amp_envelope;

    Area *add_to_algo_envelope;

    int dragging_env_index;
    std::vector<bool> selected_algo_env_points;

public:
    float envelope_length_ms;
    int active_algorithm_env_value_index = 0;
private:
    float moving_line_vertices[8];

    void draw_algorithm_envelope_point(int index, bool active = false);

    void update_amp_env_point(float time_ms, float value, int index);
    void update_envelope_times(float scaling_factor);
    Algorithm update_algorithm_env_point(float time_ms, float y_coord, int index);
};







class TouchScreenUI{
private:
    EnvelopeUI attack_envelope_ui;
    EnvelopeUI release_envelope_ui;

    bool release_envelope_active = false;
public:
    TouchScreenUI(){
        init_graphical_objects();
        attack_envelope_ui = EnvelopeUI(&Patch::get_instance().amp_attack_envelope, &Patch::get_instance().algo_attack_envelope);
        release_envelope_ui = EnvelopeUI(&Patch::get_instance().amp_release_envelope, &Patch::get_instance().algo_release_envelope);
        attack_envelope_ui.init_graphical_objects();
        release_envelope_ui.init_graphical_objects();
        attack_envelope_ui.init_envelopes(70, 460, 400, 1000.0);
        release_envelope_ui.init_envelopes(550, 460, 190, 300.0);

        attack_envelope_ui.add_amp_env_point(0.0, 0.0);
        attack_envelope_ui.add_amp_env_point(100.0, 0.8);
        attack_envelope_ui.add_amp_env_point(1000.0, 0.2);

        release_envelope_ui.add_amp_env_point(200.0, 0.0);
        release_envelope_ui.add_amp_env_point(300.0, 0.0);
        // printf("1");
        // attack_envelope_ui.add_algorithm_env_point(0.0);
        // printf("2");
        // release_envelope_ui.add_algorithm_env_point(100.0);
        // printf("3");
    }

    void initialize_operators(int number_of_operators){
        while(!operator_cursor_ids.empty()){
            operator_cursor_ids.pop_back();
        }
        while(!operator_areas.empty()){
            operator_areas.pop_back();
        }
        add_operator_area = Area(50, 180, 80, 80, 0.2);
        for(int i = 0; i < number_of_operators; i++){
            add_operator_to_algorithm(i);
        }
    }

    void add_operator_to_algorithm(int index){
        if(index >= 16){
            return;
        }
        Patch::get_instance().add_operator_to_algorithm();

        operator_cursor_ids.push_back(NO_CURSOR_ASSIGNED);
        if(index == 7){
            operator_areas.push_back(Area(add_operator_area.center_x, add_operator_area.center_y, 80, 80, 0.7));
            add_operator_area.center_x = 50;
            add_operator_area.center_y -= 130;
        }
        else{
            operator_areas.push_back(Area(add_operator_area.center_x, add_operator_area.center_y, 80, 80, 0.7));
            add_operator_area.center_x += 100;
        }
        attack_envelope_ui.select_all_algorithm_env_points(true);
        release_envelope_ui.select_all_algorithm_env_points(true);
    }

    void init_graphical_objects(){

        initialize_operators(1);
        Patch::get_instance().add_carrier(0);

        areas.push_back(&amp_env_on_frame);
        areas.push_back(&amp_env_on_area);
        areas.push_back(&amp_env_on_toggle);
        areas.push_back(&algorithm_env_on_frame);
        areas.push_back(&algorithm_env_on_area);
        areas.push_back(&algorithm_env_on_toggle);
        areas.push_back(&polyphony_env_on_frame);
        areas.push_back(&polyphony_env_on_area);
        areas.push_back(&polyphony_env_on_toggle);
        areas.push_back(&connection_button);
        areas.push_back(&conn_lock_button);
        areas.push_back(&save_button);
        areas.push_back(&load_button);
        areas.push_back(&remove_algo_env_point_area);
        areas.push_back(&select_all_algo_env_point_area);
        all_algo_env_points_selected = false;

        algorithm_env_on_frame = Area(-30, 460, 40, 40, 1.0);
        algorithm_env_on_area = Area(-30, 460, 38, 38, 0.3);
        algorithm_env_on_toggle = Area(-30, 460, 30, 30, 1.0);
                               
        //using_algorithm_envelope = get_algorithm_envelope().turn_on(using_algorithm_envelope);
        if(!Patch::get_instance().is_using_algo_envelope()){
            algorithm_env_on_toggle.color = Color(0.3, 0.3, 0.3);
        }
        
        amp_env_on_frame = Area(-30, 410, 40, 40, 1.0);
        amp_env_on_area = Area(-30, 410, 38, 38, 0.3);
        amp_env_on_toggle = Area(-30, 410, 30, 30, 1.0);

        //using_amp_envelope = get_amp_envelope().turn_on(using_amp_envelope);
        if(!Patch::get_instance().is_using_amp_envelope()){
            amp_env_on_toggle.color = Color(0.3, 0.3, 0.3);
        }

        polyphony_env_on_frame = Area(-30, 360, 40, 40, 1.0);
        polyphony_env_on_area = Area(-30, 360, 38, 38, 0.3);
        polyphony_env_on_toggle = Area(-30, 360, 30, 30, 1.0);
                               
        //using_algorithm_envelope = get_algorithm_envelope().turn_on(using_algorithm_envelope);
        if(!NoteCollections::get_instance().POLYPHONIC){
            polyphony_env_on_toggle.color = Color(0.3, 0.3, 0.3);
        }

        remove_algo_env_point_area = Area(20, 460, 30, 30, 1.0, 0.0, 0.0);
        select_all_algo_env_point_area = Area(20, 420, 30, 30, 0.0, 0.0, 1.0);

        connection_button = Area(-80, 180, 80, 80, 0.5, 0.2, 0.8);
        conn_lock_button = Area(-80, 250, 40, 40, 0.5, 0.2, 0.8);
        save_button = Area(860, 460, 60, 40, 0.0, 0.8, 0.3);
        load_button = Area(940, 460, 60, 40, 0.0, 0.2, 0.8);
    }

    void update_ui(float current_time_ms){
        Algorithm &algorithm_editor = release_envelope_active
                                    ? release_envelope_ui.get_active_envelope_value()
                                    : attack_envelope_ui.get_active_envelope_value();

        float attack_env_time = 0.0;
        float release_env_time = 0.0;
        if(NoteCollections::get_instance().active_notes.size() > 0){
            Note &note = NoteCollections::get_instance().active_notes[0];
            if(note.released){
                release_env_time = current_time_ms - note.last_event_time;
            }
            else{
                attack_env_time = current_time_ms - note.last_event_time;
            }
        }
        attack_envelope_ui.update_ui(current_time_ms, attack_env_time);
        release_envelope_ui.update_ui(current_time_ms, release_env_time);
        OperatorUI::update_time(current_time_ms);
    }

    void cursor_entry(int cursor_id, bool exit = false){
            
        if(exit){
            cursors.erase(cursor_id);
        }
        else{
            cursors[cursor_id] = Cursor(false, -1, -1);
        }
    }

    void cursor_activate(int cursor_id, int x, int y, bool deactivate = false){

        Algorithm &algorithm_editor = release_envelope_active
                                    ? release_envelope_ui.get_active_envelope_value()
                                    : attack_envelope_ui.get_active_envelope_value();

        cursors[cursor_id].on = !deactivate;
        cursors[cursor_id].setX((float)x);
        cursors[cursor_id].setY((float)y);
        //printf("multi-touch %d click\n", cursor_id);

        if(attack_envelope_ui.cursor_activate(cursor_id, x, y, algorithm_editor, deactivate)){
            release_envelope_active = false;
            // remove_algo_env_point_area.center_x = 20.0;
        }
        else if(release_envelope_ui.cursor_activate(cursor_id, x, y, algorithm_editor, deactivate)){
            release_envelope_active = true;
            // remove_algo_env_point_area.center_x = 520.0;
        }

        if(deactivate){ // BUTTON RELEASED
            for(int i = 0; i < operator_cursor_ids.size(); i++){
                if(cursor_id == operator_cursor_ids[i]){
                    operator_cursor_ids[i] = NO_CURSOR_ASSIGNED;
                    // printf("RELEASED OPERATOR EDITOR\n");
                }
            }
        }
        else{ // BUTTON PRESSED
            if(changing_connections && this->main_operator_area.is_inside(x, y) && connecting_modulation){
                Patch::get_instance().toggle_carrier(waiting_operator_index);
                if(!change_lock){
                    changing_connections = false;
                }
                connecting_modulation = false;
                waiting_operator_index = -1;
            }
            for(int i = 0; i < operator_cursor_ids.size(); i++){
                Area &oa = operator_areas[i];
                if(operator_cursor_ids[i] < 0 && oa.is_inside(x, y)){
                    if(changing_connections){
                        if(!connecting_modulation && (y < (oa.center_y - oa.height / 2.0 + 30.0))
                                && x > oa.center_x + oa.width / 2.0 - 30.0){
                            Patch::get_instance().toggle_operator_sync(i);
                            if(!change_lock){
                                changing_connections = false;
                            }
                            connecting_modulation = false;
                            waiting_operator_index = -1;
                        }
                        else if(connecting_modulation){
                            if(waiting_operator_index != i){
                                Patch::get_instance().toggle_modulation(i, waiting_operator_index);
                            }
                            if(!change_lock){
                                changing_connections = false;
                            }
                            connecting_modulation = false;
                            waiting_operator_index = -1;
                        }
                        else{
                            connecting_modulation = true;
                            waiting_operator_index = i;
                        }
                    }
                    else{
                        operator_cursor_ids[i] = cursor_id;
                        operator_frequency_change_scale = 1.6 * abs((float)x - oa.center_x) / oa.width;
                        operator_amplitude_change_scale = 1.6 * abs((float)y - oa.center_y) / oa.height;
                        if(operator_frequency_change_scale < 0.1){ operator_frequency_change_scale = 0.1; }
                        if(operator_amplitude_change_scale < 0.1){ operator_amplitude_change_scale = 0.1; }
                    }
                    break;
                }
            }
            if(add_operator_area.is_inside(x, y)){
                int new_oper_index = operator_areas.size();
                add_operator_to_algorithm(new_oper_index);
                //Patch::get_instance().add_modulation(new_oper_index - 1, new_oper_index);
                changing_connections = true;
                connecting_modulation = true;
                waiting_operator_index = new_oper_index;
            }
            else if(amp_env_on_frame.is_inside(x, y)){
                if(Patch::get_instance().toggle_amp_envelope()){
                    amp_env_on_toggle.color = Color(1.0, 1.0, 1.0);
                }
                else{
                    amp_env_on_toggle.color = Color(0.3, 0.3, 0.3);
                }
            }
            else if(algorithm_env_on_frame.is_inside(x, y)){
                if(Patch::get_instance().toggle_algo_envelope()){
                    algorithm_env_on_toggle.color = Color(1.0, 1.0, 1.0);
                    Patch::get_instance().set_fixed_algo_value(NULL);
                }
                else{
                    algorithm_env_on_toggle.color = Color(0.3, 0.3, 0.3);
                    Patch::get_instance().set_fixed_algo_value(&algorithm_editor);
                }
            }
            else if(polyphony_env_on_frame.is_inside(x, y)){
                if(NoteCollections::get_instance().toggle_polyphony()){
                    polyphony_env_on_toggle.color = Color(1.0, 1.0, 1.0);
                }
                else{
                    polyphony_env_on_toggle.color = Color(0.3, 0.3, 0.3);
                }
            }
            else if(remove_algo_env_point_area.is_inside(x, y)){
                if(release_envelope_active){
                    release_envelope_ui.remove_active_algorithm_env_point();
                }
                else{
                    attack_envelope_ui.remove_active_algorithm_env_point();
                }
            }
            else if(select_all_algo_env_point_area.is_inside(x, y)){
                all_algo_env_points_selected = !all_algo_env_points_selected;
                release_envelope_ui.select_all_algorithm_env_points(all_algo_env_points_selected);
                attack_envelope_ui.select_all_algorithm_env_points(all_algo_env_points_selected);
            }
            else if(connection_button.is_inside(x, y)){
                // TOGGLE CONNECTION MODE
                changing_connections = !changing_connections;
                change_lock = false;
                connecting_modulation = false;
                waiting_operator_index = -1;
            }
            else if(conn_lock_button.is_inside(x, y)){
                // TOGGLE CONNECTION MODE
                changing_connections = !changing_connections;
                change_lock = true;
                connecting_modulation = false;
                waiting_operator_index = -1;
            }
            else if(save_button.is_inside(x, y)){
                save_patch();
            }
            else if(load_button.is_inside(x, y)){
                load_patch();
                attack_envelope_ui.active_algorithm_env_value_index = 0;
                release_envelope_ui.active_algorithm_env_value_index = 0;
                release_envelope_active = false;
                // remove_algo_env_point_area.center_x = 20.0;
            }


            Cursor &cursor = cursors[cursor_id];
            cursor.setX(x);
            cursor.setY(y);
        }
    }

    void check_cursors(int cursor_id, int x, int y){
        Cursor &cursor = cursors[cursor_id];

        Algorithm &algorithm_editor = release_envelope_active
                                    ? release_envelope_ui.get_active_envelope_value()
                                    : attack_envelope_ui.get_active_envelope_value();

        if(attack_envelope_ui.check_cursors(cursor_id, x, y)){
            release_envelope_active = false;
            // remove_algo_env_point_area.center_x = 20.0;
        }
        else if(release_envelope_ui.check_cursors(cursor_id, x, y)){
            release_envelope_active = true;
            // remove_algo_env_point_area.center_x = 520.0;
        }

        for(int i = 0; i < operator_cursor_ids.size(); i++){
            if(cursor_id == operator_cursor_ids[i]){
                float frequency_factor_factor = 1.0;
                float amplitude_factor = 1.0;
                float phase_offset_offset = 0.0;
                if(y < operator_areas[i].center_y){
                    frequency_factor_factor = (1.0 + operator_frequency_change_scale * (float)(x - cursor.getX()) / 100.0);
                }
                else{
                    phase_offset_offset = (operator_frequency_change_scale * (float)(x - cursor.getX()) / 10.0);
                }
                amplitude_factor = (1.0 + operator_amplitude_change_scale * (float)(y - cursor.getY()) / 100.0);

                attack_envelope_ui.update_operator_for_all_selected(i, frequency_factor_factor, amplitude_factor,
                                                                    phase_offset_offset, !release_envelope_active);
                release_envelope_ui.update_operator_for_all_selected(i, frequency_factor_factor, amplitude_factor,
                                                                    phase_offset_offset, release_envelope_active);
            }
        }

        cursor.setX((float)x);
        cursor.setY((float)y);
    }

    void draw_ui(){
        Algorithm &algorithm_editor = release_envelope_active
                                    ? release_envelope_ui.get_active_envelope_value()
                                    : attack_envelope_ui.get_active_envelope_value();

        // STATIC OR NON-DISAPPEARING AREAS
        draw_areas(areas);

        // ALGORITHM OUTPUT

        draw_area(main_operator_area);

        for(int i = 0; i < operator_areas.size(); i++){
            Area &oa = operator_areas[i];
            draw_area(oa);
            draw_rectangle(oa.center_x, oa.center_y + oa.height / 4.0, oa.width - 2, oa.height / 2.0 - 2, Color(0,0,0));
        }

        if(changing_connections){

            draw_rectangle(connection_button.center_x, connection_button.center_y, connection_button.width - 20, connection_button.height - 20, Color(1.0, 1.0, 1.0));
            if(change_lock){
                draw_rectangle(conn_lock_button.center_x, conn_lock_button.center_y - 20, conn_lock_button.width - 20, conn_lock_button.height + 20, Color(1.0, 1.0, 1.0));
            }

            if(connecting_modulation){
                if(Patch::get_instance().is_carrier(waiting_operator_index)){
                    // RED TO DELETE
                    draw_rectangle(main_operator_area.center_x, main_operator_area.center_y, main_operator_area.width - 20.0, main_operator_area.height - 20.0, Color(0.8,0.1,0.1));
                }
                else{
                    // YELLOW TO ADD
                    draw_rectangle(main_operator_area.center_x, main_operator_area.center_y, main_operator_area.width - 20.0, main_operator_area.height - 20.0, Color(0.8,0.8,0.1));
                }
            }

            for(int i = 0; i < operator_areas.size(); i++){
                Area &oa = operator_areas[i];
                draw_rectangle(oa.center_x - (oa.width / 2.0 - 10.0), oa.center_y - 5.0, 20.0, 10.0, Color(0.6,0.6,0.6));
                draw_rectangle(oa.center_x - (oa.width / 2.0 - 10.0), oa.center_y + 5.0, 20.0, 10.0, Color(0.1,0.1,0.1));

                if(connecting_modulation){
                    if(waiting_operator_index != i){
                        if(Patch::get_instance().modulation_exists(i, waiting_operator_index)){
                            // RED TO DELETE
                            draw_rectangle(oa.center_x, oa.center_y, oa.width - 20.0, oa.height - 20.0, Color(0.8,0.1,0.1));
                        }
                        else{
                            // YELLOW TO ADD
                            draw_rectangle(oa.center_x, oa.center_y, oa.width - 20.0, oa.height - 20.0, Color(0.8,0.8,0.1));
                        }
                    }
                    else{
                        // GREY IF THIS ONE IS WAITING
                        draw_rectangle(oa.center_x, oa.center_y, oa.width - 20.0, oa.height - 20.0, Color(0.2,0.2,0.2));
                    }
                }
                else{
                    // BLUE
                    draw_rectangle(oa.center_x, oa.center_y, oa.width - 20.0, oa.height - 20.0, Color(0.1,0.1,0.8));
                }

                if(algorithm_editor.operators[i].sync_to_base_frequency){
                    draw_rectangle(oa.center_x + (oa.width / 2.0 - 15.0), oa.center_y - (oa.height / 2.0 - 15.0), 30.0, 30.0, Color(0.2,0.8,0.4));
                }
                else{
                    draw_rectangle(oa.center_x + (oa.width / 2.0 - 15.0), oa.center_y - (oa.height / 2.0 - 15.0), 30.0, 30.0, Color(0.4,0.2,0.4));
                }

            }
        }
        glPushMatrix();
            glTranslatef(main_operator_area.center_x, main_operator_area.center_y, 0.0);
            glScalef(main_operator_area.width / 10.0, main_operator_area.height, 1.0);
            // OperatorUI::draw_modulated_operator(&algorithm_editor.operators[0], algorithm_editor.operators.size(), 10.0, false);
            OperatorUI::draw_whole_patch(&algorithm_editor.operators[0], 10.0, false);
        glPopMatrix();

        // OPERATORS

        float order_vertices[8];
        for(int i = 0; i < Patch::get_instance().carriers.size(); i++){
            Area &oa = operator_areas[Patch::get_instance().carriers[i]];
            order_vertices[0] = order_vertices[6] = oa.center_x - oa.width / 2.0;
            if(Patch::get_instance().carriers[i] < 8){
                order_vertices[2] = order_vertices[4] = order_vertices[0] - 3.0;
            }
            else{
                order_vertices[2] = order_vertices[4] = order_vertices[0] - 7.0;
            }
            order_vertices[1] = order_vertices[3] = oa.center_y;
            order_vertices[5] = order_vertices[7] = main_operator_area.center_y - main_operator_area.height / 2.0;
            draw_line_strip(order_vertices, 4, 1.0);
        }

        for(int i = 0; i < operator_areas.size(); i++){
            bool editing = (operator_cursor_ids[i] != NO_CURSOR_ASSIGNED);
            Area &oa = operator_areas[i];

            if(algorithm_editor.operators[i].sync_to_base_frequency){
                draw_rectangle(oa.center_x + (oa.width / 2.0 - 5.0), oa.center_y - (oa.height / 2.0 - 5.0), 10.0, 10.0, Color(0.2,0.8,0.4));
            }
            else{
                draw_rectangle(oa.center_x + (oa.width / 2.0 - 5.0), oa.center_y - (oa.height / 2.0 - 5.0), 10.0, 10.0, Color(0.4,0.2,0.4));
            }

            glPushMatrix();
                glTranslatef(oa.center_x, oa.center_y - oa.height / 4.0, 0.0);
                glScalef(oa.width / 2.0, oa.height / 2.0, 1.0);
                OperatorUI::draw_operator(&algorithm_editor.operators[i], 2.0, editing);
            glPopMatrix();
            glPushMatrix();
                glTranslatef(oa.center_x, oa.center_y + oa.height / 4.0, 0.0);
                glScalef(oa.width / 2.0, oa.height / 2.0, 1.0);
                //OperatorUI::draw_modulated_operator(&algorithm_editor.operators[i], operator_areas.size() - i, 2.0, editing);
                OperatorUI::draw_operator_with_modulation(&algorithm_editor.operators[0], i, 2.0, editing);
            glPopMatrix();

    // OPERATOR CONNECTIONS

            for(int j = 0; j < Patch::get_instance().modulation_order[i].size(); j++){
                Area &oa2 = operator_areas[Patch::get_instance().modulation_order[i][j]];
                order_vertices[0] = order_vertices[2] = oa.center_x + oa.width / 4.0;
                order_vertices[4] = order_vertices[6] = oa2.center_x - oa2.width / 4.0;
                if(oa.center_y > oa2.center_y){
                    order_vertices[1] = oa.center_y - oa.height / 2.0;
                    order_vertices[3] = order_vertices[5] = oa.center_y - oa.height / 2.0 - 2.0 * (i % 8) + 1.0;
                }
                else{
                    order_vertices[1] = oa.center_y + oa.height / 2.0;
                    order_vertices[3] = order_vertices[5] = oa.center_y + oa.height / 2.0 + 2.0 * (i % 8) + 1.0;
                }
                if(oa.center_y < oa2.center_y){
                    order_vertices[7] = oa2.center_y - oa2.height / 2.0;
                }
                else{
                    order_vertices[7] = oa2.center_y + oa2.height / 2.0;
                }
                draw_line_strip(order_vertices, 4, 1.0); // 0.5 + 0.04 * i);
            }

        ///////

        }
        draw_area(add_operator_area);



        order_vertices[0] = operator_areas[0].width / -4.0 - 5.0;
        order_vertices[2] = order_vertices[0] + 5.0;
        order_vertices[4] = order_vertices[2] + 5.0;

        order_vertices[1] = order_vertices[5] = -5.0;
        order_vertices[3] = 0.0;
        
        for(int i = 0; i < operator_areas.size(); i++){
            Area &oa = operator_areas[i];
            glPushMatrix();
                glTranslatef(oa.center_x, oa.center_y, 0.0);
                for(int rot = 0; rot < 3; rot++){
                    if(rot == 1){
                        glScalef(1.0, -1.0, 1.0);
                    }
                    else if(rot == 2){
                        glRotatef(90.0, 0.0, 0.0, 1.0);
                        glTranslatef(oa.height / 4.0, 0.0, 0.0);
                    }
                    glPushMatrix();
                        glTranslatef(0.0, oa.height / 2.0, 0.0);
                        draw_line_strip(order_vertices, 3, 1.0);
                        if(rot != 2){
                            glRotatef(180.0, 0.0, 0.0, 1.0);
                            glTranslatef(0.0, 5.0, 0.0);
                            draw_line_strip(order_vertices, 3, 1.0);
                        }
                    glPopMatrix();
                }
            glPopMatrix();
        }


        attack_envelope_ui.draw_ui(algorithm_editor);
        release_envelope_ui.draw_ui(algorithm_editor);

        for(int i = 0; i < operator_cursor_ids.size(); i++){
            if(operator_cursor_ids[i] != NO_CURSOR_ASSIGNED){
                glPushMatrix();
                    glTranslatef(operator_areas[i].center_x - (operator_areas[i].width / 2),
                                operator_areas[i].center_y + (operator_areas[i].height / 2), 0.0);
                    glPushMatrix();
                        glScalef(1.0 / operator_frequency_change_scale, 20.0, 1.0);
                        draw_ruler(std::max((double)(operator_areas[i].width * operator_frequency_change_scale), 11.0));//42.0));
                    glPopMatrix();
                    glPushMatrix();
                        glRotatef(-90.0, 0.0, 0.0, 1.0);
                        glScalef(1.0 / operator_amplitude_change_scale, -20.0, 1.0);
                        draw_ruler(std::max((double)(operator_areas[i].height * operator_amplitude_change_scale), 11.0));//42.0));
                    glPopMatrix();
                glPopMatrix();
            }
        }
    }

private:
    std::map<int, Cursor> cursors;
    std::vector<int> operator_cursor_ids;

    bool changing_connections = false;
    bool change_lock = false;
    bool connecting_modulation = false;
    int waiting_operator_index = -1;

    Area main_operator_area = Area(400, 270, 780, 60, 0.2);  // MOVE ALL OF THIS INITIALIZATION INTO OPERATION

    std::vector<Area> operator_areas;

    Area add_operator_area;

    std::vector<Area *> areas;

    Area remove_algo_env_point_area;
    Area select_all_algo_env_point_area;
    bool all_algo_env_points_selected;

    Area amp_env_on_frame;
    Area amp_env_on_area;
    Area amp_env_on_toggle;

    Area algorithm_env_on_frame;
    Area algorithm_env_on_area;
    Area algorithm_env_on_toggle;

    Area polyphony_env_on_frame;
    Area polyphony_env_on_area;
    Area polyphony_env_on_toggle;

    float operator_amplitude_change_scale = 1.0;
    float operator_frequency_change_scale = 1.0;

    Area connection_button;
    Area conn_lock_button;
    Area save_button;
    Area load_button;

    void save_patch(){
        printf("SAVE\nEnter patch name (15 chars max, no spaces): ");
        char patch_name[16];
        std::cin >> patch_name;
        patch_name[15] = '\0';
        printf("This is your patch name: %s\nSAVING!\n", patch_name);
        std::ofstream fout("save_file.txt", std::ios::app);

    // NAME
        fout << patch_name << "\n";

    // OPERATOR COUNT IN ALGORITHM
        fout << operator_cursor_ids.size() << "\n";

        for(int i = 0; i < operator_cursor_ids.size(); i++){
            fout << Patch::get_instance().sync_operators[i] << " ";
        }
        fout << "\n";


    // ATTACK ALGORITHM ENVELOPE
        Envelope<Algorithm> &att_algo_env = Patch::get_instance().algo_attack_envelope;
        fout << att_algo_env.env_values.size() << "\n";
        for(int i = 0; i < att_algo_env.env_values.size(); i++){
            fout << att_algo_env.env_values[i]->time_ms << "\n";
            std::vector<OperatorValue> &ops = att_algo_env.env_values[i]->value.operators;
            for(int j = 0; j < ops.size(); j++){
                fout << ops[j].amplitude << "\t" << ops[j].frequency_factor << "\n";
            }
        }

    // RELEASE ALGORITHM ENVELOPE
        Envelope<Algorithm> &rel_algo_env = Patch::get_instance().algo_release_envelope;
        fout << rel_algo_env.env_values.size() << "\n";
        for(int i = 0; i < rel_algo_env.env_values.size(); i++){
            fout << rel_algo_env.env_values[i]->time_ms << "\n";
            std::vector<OperatorValue> &ops = rel_algo_env.env_values[i]->value.operators;
            for(int j = 0; j < ops.size(); j++){
                fout << ops[j].amplitude << "\t" << ops[j].frequency_factor << "\n";
            }
        }

    // ATTACK AMP ENVELOPE
        Envelope<float> &att_amp_env = Patch::get_instance().amp_attack_envelope;
        fout << att_amp_env.env_values.size() << "\n";
        for(int i = 0; i < att_amp_env.env_values.size(); i++){
            fout << att_amp_env.env_values[i]->time_ms << "\t" << att_amp_env.env_values[i]->value << "\n";
        }

    // RELEASE AMP ENVELOPE
        Envelope<float> &rel_amp_env = Patch::get_instance().amp_release_envelope;
        fout << rel_amp_env.env_values.size() << "\n";
        for(int i = 0; i < rel_amp_env.env_values.size(); i++){
            fout << rel_amp_env.env_values[i]->time_ms << "\t" << rel_amp_env.env_values[i]->value << "\n";
        }

    // USING ENVELOPES AND POLYPHONY
        if(Patch::get_instance().is_using_algo_envelope()){
            fout << "1\t";
        }
        else{
            fout << "0\t";
        }
        if(Patch::get_instance().is_using_amp_envelope()){
            fout << "1\t";
        }
        else{
            fout << "0\t";
        }
        if(NoteCollections::get_instance().POLYPHONIC){
            fout << "1\n";
        }
        else{
            fout << "0\n";
        }

        fout << "connections\n";
        fout << Patch::get_instance().carriers.size();
        for(int i = 0; i < Patch::get_instance().carriers.size(); i++){
            fout << " " << Patch::get_instance().carriers[i];
        }
        fout << "\n";
        for(int i = 0; i < Patch::get_instance().modulation_order.size(); i++){
            fout << " " << Patch::get_instance().modulation_order[i].size();
            for(int j = 0; j < Patch::get_instance().modulation_order[i].size(); j++){
                fout << " " << Patch::get_instance().modulation_order[i][j];
            }
            fout << "\n";
        }

        fout.close();
        printf("SAVED!\n");
    }

    void load_patch(){
        printf("LOAD\nEnter patch name: ");
        char patch_name[16];
        std::cin >> patch_name;
        patch_name[15] = '\0';
        std::ifstream fin("save_file.txt");

    // NAME
        char searcher[15];
        do{
            fin >> searcher;
        }while(!fin.eof() && strcmp(searcher, patch_name) != 0);
        if(fin.eof()){
            printf("Could not find patch with name: %s\nLEAVING!\n", patch_name);
            return;
        }

    // OPERATOR COUNT IN ALGORITHM
        int operator_count;
        fin >> operator_count;

        initialize_operators(operator_count);

        for(int i = 0; i < operator_count; i++){
            fin >> Patch::get_instance().sync_operators[i];
        }

        int vector_size;

    // ATTACK ALGORITHM ENVELOPE
        Envelope<Algorithm> &att_algo_env = Patch::get_instance().algo_attack_envelope;
        while(!att_algo_env.env_values.empty()){ // REMOVE AND CLEANUP CURRENT VALUES
            delete att_algo_env.env_values.back();
            att_algo_env.env_values.pop_back();
        }
        fin >> vector_size;
        for(int i = 0; i < vector_size; i++){
            float time_ms;
            fin >> time_ms;
            att_algo_env.env_values.push_back(new EnvelopeValue<Algorithm>(Algorithm(0), time_ms));
            std::vector<OperatorValue> &ops = att_algo_env.env_values.back()->value.operators;
            for(int j = 0; j < operator_count; j++){
                float ampl, freqf;
                fin >> ampl;
                fin >> freqf;
                ops.push_back(OperatorValue(freqf, ampl, 0.0, (Patch::get_instance().sync_operators[j] >= 0)));
            }
        }

    // RELEASE ALGORITHM ENVELOPE
        Envelope<Algorithm> &rel_algo_env = Patch::get_instance().algo_release_envelope;
        while(!rel_algo_env.env_values.empty()){ // REMOVE AND CLEANUP CURRENT VALUES
            delete rel_algo_env.env_values.back();
            rel_algo_env.env_values.pop_back();
        }
        fin >> vector_size;
        for(int i = 0; i < vector_size; i++){
            float time_ms;
            fin >> time_ms;
            rel_algo_env.env_values.push_back(new EnvelopeValue<Algorithm>(Algorithm(0), time_ms));
            std::vector<OperatorValue> &ops = rel_algo_env.env_values.back()->value.operators;
            for(int j = 0; j < operator_count; j++){
                float ampl, freqf;
                fin >> ampl;
                fin >> freqf;
                ops.push_back(OperatorValue(freqf, ampl, 0.0, (Patch::get_instance().sync_operators[j] >= 0)));
            }
        }

    // ATTACK AMP ENVELOPE
        Envelope<float> &att_amp_env = Patch::get_instance().amp_attack_envelope;
        while(!att_amp_env.env_values.empty()){ // REMOVE AND CLEANUP CURRENT VALUES
            delete att_amp_env.env_values.back();
            att_amp_env.env_values.pop_back();
        }
        fin >> vector_size;
        for(int i = 0; i < vector_size; i++){
            float time_ms, amp_val;
            fin >> time_ms;
            fin >> amp_val;
            att_amp_env.env_values.push_back(new EnvelopeValue<float>(amp_val, time_ms));
        }
        attack_envelope_ui.envelope_length_ms = att_amp_env.env_values[vector_size - 1]->time_ms;

    // RELEASE AMP ENVELOPE
        Envelope<float> &rel_amp_env = Patch::get_instance().amp_release_envelope;
        while(!rel_amp_env.env_values.empty()){ // REMOVE AND CLEANUP CURRENT VALUES
            delete rel_amp_env.env_values.back();
            rel_amp_env.env_values.pop_back();
        }
        fin >> vector_size;
        for(int i = 0; i < vector_size; i++){
            float time_ms, amp_val;
            fin >> time_ms;
            fin >> amp_val;
            rel_amp_env.env_values.push_back(new EnvelopeValue<float>(amp_val, time_ms));
        }
        release_envelope_ui.envelope_length_ms = rel_amp_env.env_values[vector_size - 1]->time_ms;

    // USING ENVELOPES AND POLYPHONY
        int us_alg_env, us_amp_env, poly;
        fin >> us_alg_env;
        fin >> us_amp_env;
        fin >> poly;
        if((us_alg_env == 1) != Patch::get_instance().is_using_algo_envelope()){
            Patch::get_instance().toggle_algo_envelope();
        }
        if((us_amp_env == 1) != Patch::get_instance().is_using_amp_envelope()){
            Patch::get_instance().toggle_algo_envelope();
        }
        if((poly == 1) != NoteCollections::get_instance().POLYPHONIC){
            NoteCollections::get_instance().toggle_polyphony();
        }

        bool connections_missing = true;
        if(!fin.eof()){
            fin >> patch_name;
            if(!fin.eof() && strcmp(patch_name, "connections") == 0){
                connections_missing = false;
            }
        }
        if(connections_missing){
            Patch::get_instance().carriers.clear();
            Patch::get_instance().carriers.push_back(0);
            Patch::get_instance().modulation_order.clear();
            for(int i = 0; i < operator_count - 1; i++){
                Patch::get_instance().modulation_order.push_back(std::vector<int>(1, i + 1));
            }
            Patch::get_instance().modulation_order.push_back(std::vector<int>(0));
        }
        else{
            Patch::get_instance().carriers.clear();
            int carrier_count, carrier_index;
            fin >> carrier_count;
            for(int i = 0; i < carrier_count; i++){
                fin >> carrier_index;
                Patch::get_instance().carriers.push_back(carrier_index);
            }
            Patch::get_instance().modulation_order.clear();
            int mod_count, mod_index;
            for(int i = 0; i < operator_count; i++){
                Patch::get_instance().modulation_order.push_back(std::vector<int>(0));
                fin >> mod_count;
                for(int j = 0; j < mod_count; j++){
                    fin >> mod_index;
                    Patch::get_instance().modulation_order[i].push_back(mod_index);
                }
            }
        }

        fin.close();
        printf("LOADED!\n");

        if(Patch::get_instance().is_using_amp_envelope()){
            amp_env_on_toggle.color = Color(1.0, 1.0, 1.0);
        }
        else{
            amp_env_on_toggle.color = Color(0.3, 0.3, 0.3);
        }
        if(Patch::get_instance().is_using_algo_envelope()){
            algorithm_env_on_toggle.color = Color(1.0, 1.0, 1.0);
            Patch::get_instance().set_fixed_algo_value(NULL);
        }
        else{
            algorithm_env_on_toggle.color = Color(0.3, 0.3, 0.3);
            Patch::get_instance().set_fixed_algo_value(&attack_envelope_ui.get_active_envelope_value());
        }
        if(NoteCollections::get_instance().POLYPHONIC){
            polyphony_env_on_toggle.color = Color(1.0, 1.0, 1.0);
        }
        else{
            polyphony_env_on_toggle.color = Color(0.3, 0.3, 0.3);
        }
    }
};

#endif //DAUDASYNTHINN_UI_H_82436482734