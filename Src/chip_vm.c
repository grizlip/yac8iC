#include "chip_vm.h"
#include <stdio.h>
#include <stdlib.h>
#define INSTRUCTIONS_COUNT 34

typedef struct instruction {
    uint16_t opcode;
    uint16_t mask;
    void (*execute_function)(chip_vm_state* state, uint16_t args);
} instruction;

void instruction_clear_screen(chip_vm_state* state, uint16_t args);
void instruction_stack_pop(chip_vm_state* state, uint16_t args);
void instruction_jump(chip_vm_state* state, uint16_t args);
void instruction_stack_push(chip_vm_state* state, uint16_t args);
void instruction_skip_value_equal(chip_vm_state* state, uint16_t args);
void instruction_skip_value_not_equal(chip_vm_state* state, uint16_t args);
void instruction_skip_register_equal(chip_vm_state* state, uint16_t args);
void instruction_set_register_immediate(chip_vm_state* state, uint16_t args);
void instruction_add_register_immediate(chip_vm_state* state, uint16_t args);
void instruction_set_register_register(chip_vm_state* state, uint16_t args);
void instruction_or_register_register(chip_vm_state* state, uint16_t args);
void instruction_and_register_register(chip_vm_state* state, uint16_t args);
void instruction_xor_register_register(chip_vm_state* state, uint16_t args);
void instruction_add_register_register(chip_vm_state* state, uint16_t args);
void instruction_sub_register_register(chip_vm_state* state, uint16_t args);
void instruction_shr_register_register(chip_vm_state* state, uint16_t args);
void instruction_subn_register_register(chip_vm_state* state, uint16_t args);
void instruction_lhr_register_register(chip_vm_state* state, uint16_t args);
void instruction_skip_not_equal_register_register(chip_vm_state* state, uint16_t args);
void instruction_load_i_register(chip_vm_state* state, uint16_t args);
void instruction_jump_with_offset(chip_vm_state* state, uint16_t args);
void instruction_random(chip_vm_state* state, uint16_t args);
void instruction_draw(chip_vm_state* state, uint16_t args);
void instruction_skip_key_pressed(chip_vm_state* state, uint16_t args);
void instruction_skip_key_not_pressed(chip_vm_state* state, uint16_t args);
void instruction_read_delay(chip_vm_state* state, uint16_t args);
void instruction_wait_key_pressed(chip_vm_state* state, uint16_t args);
void instruction_set_delay(chip_vm_state* state, uint16_t args);
void instruction_set_sound(chip_vm_state* state, uint16_t args);
void instruction_set_i(chip_vm_state* state, uint16_t args);
void instruction_font_character(chip_vm_state* state, uint16_t args);
void instruction_binary_decimal_conversion(chip_vm_state* state, uint16_t args);
void instruction_write_memory(chip_vm_state* state, uint16_t args);
void instruction_load_memory(chip_vm_state* state, uint16_t args);
static instruction instructions[INSTRUCTIONS_COUNT] = 
{
    {0x00E0,0xFFFF, instruction_clear_screen},
    {0x00EE,0xFFFF, instruction_stack_pop},
    {0x1000,0xF000, instruction_jump},
    {0x2000,0xF000, instruction_stack_push},
    {0x3000,0xF000, instruction_skip_value_equal},
    {0x4000,0xF000, instruction_skip_value_not_equal},
    {0x5000,0xF00F, instruction_skip_register_equal},
    {0x6000,0xF000, instruction_set_register_immediate},
    {0x7000,0xF000, instruction_add_register_immediate},
    {0x8000,0xF00F, instruction_set_register_register},
    {0x8001,0xF00F, instruction_or_register_register},
    {0x8002,0xF00F, instruction_and_register_register},
    {0x8003,0xF00F, instruction_xor_register_register},
    {0x8004,0xF00F, instruction_add_register_register},
    {0x8005,0xF00F, instruction_sub_register_register},
    {0x8006,0xF00F, instruction_shr_register_register},
    {0x8007,0xF00F, instruction_subn_register_register},
    {0x800E,0xF00F, instruction_lhr_register_register},
    {0x9000,0xF00F, instruction_skip_not_equal_register_register},
    {0xA000,0xF000, instruction_load_i_register},
    {0xB000,0xF000, instruction_jump_with_offset},
    {0xC000,0xF000, instruction_random},
    {0xD000,0xF000, instruction_draw},
    {0xE09E,0xF0FF, instruction_skip_key_pressed},
    {0xE0A1,0xF0FF, instruction_skip_key_not_pressed},
    {0xF007,0xF0FF, instruction_read_delay},
    {0xF015,0xF0FF, instruction_set_delay},
    {0xF018,0xF0FF, instruction_set_sound},
    {0xF00A,0xF0FF, instruction_wait_key_pressed},
    {0xF01E,0xF0FF, instruction_set_i},
    {0xF029,0xF0FF, instruction_font_character},
    {0xF033,0xF0FF, instruction_binary_decimal_conversion},
    {0xF055,0xF0FF, instruction_write_memory},
    {0xF065,0xF0FF, instruction_load_memory}
};

chip_vm_state* vm_create_state(uint8_t* memory,size_t size) {
    chip_vm_state* vm = NULL;
    if(size <= (4096 - 512)) {
        vm = (chip_vm_state*)malloc(sizeof(chip_vm_state));
        vm->program_counter = 0x200;
        for(int i =0;i<80;i++) {
            vm->memory[i] = font[i];
        }

        for(size_t i = 0;i<size;i++) {
            vm->memory[0x200+i] = memory[i];
        }
        for(int i =0;i<64;i++) {
            for(int j =0;j<32;j++) {
                vm->vm_surface[i][j] = false;
            }
        }
        vm->stack_pointer = -1;
    }
    return vm;
}

void vm_destroy_state(chip_vm_state* state) {
    if(state != NULL) {
        free(state);
    }
}

bool vm_execute(chip_vm_state* state, uint8_t cycles_per_frame) {
    if(state != NULL) {
        for(uint8_t cycle = 0; cycle < cycles_per_frame; cycle++) {
            if(state->program_counter < 4096) {
                uint16_t instruction = (state->memory[state->program_counter] <<8 )| state->memory[state->program_counter+1];
                for(int i = 0;i<INSTRUCTIONS_COUNT;i++) {
                    if((instruction & instructions[i].mask) == instructions[i].opcode) {
                        uint16_t args = instruction & (instructions[i].mask ^ 0xFFFF);
                        instructions[i].execute_function(state,args);
                        state->program_counter+=2;

                        break;
                    }
                }
            }
        }
    }
    return false;
}

void vm_tick(chip_vm_state* state) {
    if(state->dt >0) {
        state->dt -=1;
    }
    if(state->st >0) {
        state->st -=1;
    }
}

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

