#ifndef CHIP_INSTRUCTIONS_H
#define CHIP_INSTRUCTIONS_H
#include "chip_vm.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
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
#endif
