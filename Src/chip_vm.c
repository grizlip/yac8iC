#include "chip_vm.h"
#include "chip_instructions.h"
#include <stdio.h>
#include <stdlib.h>
#define INSTRUCTIONS_COUNT 34

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
