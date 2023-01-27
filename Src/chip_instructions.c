#include "chip_instructions.h"

void instruction_clear_screen(chip_vm_state* state, uint16_t args) {
    for(int i =0;i<64;i++) {
        for(int j =0;j<32;j++) {
            state->vm_surface[i][j] = false;
        }
    }
}

void instruction_stack_pop(chip_vm_state* state, uint16_t args) {

    if((state->stack_pointer >= 0) && (state->stack_pointer<=15)) {
        state->program_counter = state->stack[state->stack_pointer]-2;
        state->stack_pointer -= 1;
    }
    else {
        //TODO: error!!
    }
}

void instruction_stack_push(chip_vm_state* state, uint16_t args) {
    int8_t tmp_stack_pointer = state->stack_pointer+1; 
    if((tmp_stack_pointer < 16) &&  (tmp_stack_pointer>-1)) {
        state->stack[tmp_stack_pointer] = state->program_counter+2;
        state->stack_pointer = tmp_stack_pointer;
        state->program_counter = args - 2; //to account for change in the execute function
    }
    else {
        //TODO: error!
    }
}

void instruction_jump(chip_vm_state* state, uint16_t args) {
    if(args < 4096) {
        state->program_counter = args - 2; //to account for change in the execute function
    }
    else
    {
        //TODO: error!
    }
}

void instruction_skip_value_equal(chip_vm_state* state, uint16_t args) {
    uint8_t register_index = (args & 0x0F00) >> 8;
    uint8_t register_value = state->registers[register_index];
    uint8_t compare_to_value = (args & 0x00FF);
    if(register_value == compare_to_value)
    {
        state->program_counter += 2;
    }
}

void instruction_skip_value_not_equal(chip_vm_state* state, uint16_t args) {
    uint8_t register_index = (args & 0x0F00) >> 8;
    uint8_t register_value = state->registers[register_index];
    uint8_t compare_to_value = (args & 0x00FF);
    if(register_value != compare_to_value)
    {
        state->program_counter += 2;
    }
}

void instruction_skip_register_equal(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    if(state->registers[reg_x_index] == state->registers[reg_y_index]) {
        state->program_counter +=2;
    }
}

void instruction_set_register_immediate(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t new_register_value = args & 0x00FF;
    state->registers[reg_x_index] = new_register_value;
}

void instruction_add_register_immediate(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t new_register_value = args & 0x00FF;
    //since we have to unsigned values I think that oveflow in this case will result in wrapping modulo 2^n
    //which is exacly wahat we want
    uint8_t reg_value = state->registers[reg_x_index];
    state->registers[reg_x_index] += new_register_value;    
    uint16_t x = reg_value;
    uint16_t y = new_register_value;
    state->registers[0xF] = x + y > 255 ? 1 : 0;
}

void instruction_set_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_y_index];
}

void instruction_or_register_register (chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_x_index] | state->registers[reg_y_index];
    state->registers[0xF] = 0;
}

void instruction_and_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_x_index] & state->registers[reg_y_index];
    state->registers[0xF] = 0;
}

void instruction_xor_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_x_index] ^ state->registers[reg_y_index];
    state->registers[0xF] = 0;
}

void instruction_add_register_register(chip_vm_state* state, uint16_t args) { 
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    uint16_t x = (uint16_t)state->registers[reg_x_index];
    uint16_t y = (uint16_t)state->registers[reg_y_index];
    //test for overflow
    state->registers[reg_x_index] = state->registers[reg_x_index] + state->registers[reg_y_index];
    state->registers[0xF] = x + y > 255 ? 1 : 0;
}

void instruction_sub_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_x_index] - state->registers[reg_y_index];
    //test for underflow
    state->registers[0xF] = state->registers[reg_x_index] > state->registers[reg_y_index];
}

void instruction_shr_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_y_index];
    uint8_t x_orginal = state->registers[reg_x_index] ;
    state->registers[reg_x_index] = x_orginal >> 1;
    state->registers[0xF] = x_orginal & 0x1;
}

void instruction_subn_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;

    uint8_t x_orginal = state->registers[reg_x_index];
    uint8_t y_orginal = state->registers[reg_y_index];

    state->registers[reg_x_index] = y_orginal - x_orginal;
    state->registers[0xF] = x_orginal < y_orginal ? 1 : 0;
}

void instruction_lhr_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    state->registers[reg_x_index] = state->registers[reg_y_index];
    uint8_t x_orginal = state->registers[reg_x_index] ;
    state->registers[reg_x_index] = x_orginal << 1;
    state->registers[0xF] = (x_orginal & 0x80) >> 7;
}

void instruction_skip_not_equal_register_register(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    if(state->registers[reg_x_index] != state->registers[reg_y_index])
    {
        state->program_counter+=2;
    }
}

void instruction_load_i_register(chip_vm_state* state, uint16_t args) {
    state->i_register = args & 0x0FFF;
}

void instruction_jump_with_offset(chip_vm_state* state, uint16_t args) {
    uint16_t base = (uint16_t)state->registers[0];
    uint16_t offset = (uint16_t)(args & 0x0FFF);
    state->program_counter = base + offset;
}

void instruction_random(chip_vm_state* state, uint16_t args) {
    uint8_t kk = args & 0x00FF;
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t rand_value = rand() % 255;
    state->registers[reg_x_index] = rand_value & kk;
}

void instruction_draw(chip_vm_state* state, uint16_t args) {
    state->registers[0xF] = 0;
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_y_index = (args & 0x00F0)>>4;
    uint8_t sprite_length = (args & 0x000F);
    uint8_t pos_x = state->registers[reg_x_index];
    uint8_t pos_y = state->registers[reg_y_index];

    uint8_t row_beginning = (pos_x % 64);
    uint8_t y = (pos_y % 32);
    uint8_t x = row_beginning;
    uint16_t sprite_end = sprite_length + state->i_register;
    for(int i = state->i_register; i < sprite_end;i++)
    {
        if(y < 32)
        {
            uint8_t sprite_row = state->memory[i];
            for(int row_index = 7; row_index >=0 ; row_index--)
            {
                bool bit = (sprite_row >> row_index) & 0x01;
                if (x < 64)
                {
                    if(state->vm_surface[x][y] && bit)
                    {
                        state->vm_surface[x][y] = false;
                        state->registers[0xF] = 1;
                    }
                    else if(!state->vm_surface[x][y] && bit)
                    {
                        state->vm_surface[x][y] = true;
                    }
                    x += 1;
                }
                else
                {
                    break;
                }
            }
            x = row_beginning;
            y += 1;
        }
        else
        {
            break;
        }
    }
}

void instruction_skip_key_pressed(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t key_index = state->registers[reg_x_index];
    if(key_index >0xF) {
        //TODO: error!
    }
    else {
        if((state->pressed_keys & (uint16_t)(1 << key_index)) > 0) {
            state->program_counter +=2;
        }
    }
}

void instruction_skip_key_not_pressed(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t key_index = state->registers[reg_x_index];
    if(key_index >0xF) {
        //TODO: error!
    }
    else {
        if((state->pressed_keys & (uint16_t)(1 << key_index)) == 0) {
            state->program_counter +=2;
        }
    }
}

void instruction_read_delay(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    state->registers[reg_x_index] = state->dt;
}

void instruction_wait_key_pressed(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;

    if(state->pressed_keys == 0) {
        state->program_counter -=2;
    }
    else {
        uint8_t i=0;
        for(;i<16;i++)
        {
            if((state->pressed_keys & (uint16_t)(1 << i)) == 0) {
                break;
            }
        }
        state->registers[reg_x_index] = i;
    }
}

void instruction_set_delay(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    state->dt = state->registers[reg_x_index];
}

void instruction_set_sound(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    state->st = state->registers[reg_x_index];
}

void instruction_set_i(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    state->i_register += state->registers[reg_x_index];

    if((state->i_register & 0xF000) >0) {
        state->registers[0xF] = 1;
    }
}

void instruction_font_character(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    state->i_register = (uint16_t)(state->registers[reg_x_index] * 5);
}

void instruction_binary_decimal_conversion(chip_vm_state* state, uint16_t args) {
    uint8_t reg_x_index = (args & 0x0F00)>>8;
    uint8_t reg_value = state->registers[reg_x_index];

    for(int i = 2; i >=0; i--) {
        if(state->i_register + i >=4096) {
            //TODO: error!
        }
        else {
            state->memory[state->i_register + i] = (uint8_t)(reg_value % 10);
            reg_value = (uint8_t)(reg_value/10);

        }
    }
}

void instruction_write_memory(chip_vm_state* state, uint16_t args) {
    uint8_t last_register_index = (args & 0x0F00)>>8;
    for(int i =0;i<=last_register_index;i++) {
        state->memory[state->i_register + i] = state->registers[i];
    }
    state->i_register += (uint16_t)(last_register_index +1);
}

void instruction_load_memory(chip_vm_state* state, uint16_t args) {
    uint8_t last_register_index = (args & 0x0F00)>>8;
    for(int i =0;i<=last_register_index;i++) {
        state->registers[i] = state->memory[state->i_register + i];
    }
    state->i_register += (uint16_t)(last_register_index +1);
}

