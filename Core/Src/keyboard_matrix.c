#include "keyboard_matrix.h"
#include "keyboard_state.h"
#include "keymaps.h"
#include "main.h"
#include "stm32f1xx_hal.h"

#define KEYBOARD_PULL 0 // 0 for PULLDOWN

static uint8_t matrix[MATRIX_ROWS];
static uint8_t matrix_debouncing[MATRIX_COLS];
static uint8_t matrix_prev[MATRIX_ROWS];

KEY_DEB keyboard_debouncing;

// Pin definitions for rows and cols
static GPIO_TypeDef* matrix_rows_port[MATRIX_ROWS] = {
    ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port, ROW4_GPIO_Port,
    ROW5_GPIO_Port, ROW6_GPIO_Port, ROW7_GPIO_Port, ROW8_GPIO_Port
};

static uint16_t matrix_rows_pin[MATRIX_ROWS] = {
    ROW1_Pin, ROW2_Pin, ROW3_Pin, ROW4_Pin,
    ROW5_Pin, ROW6_Pin, ROW7_Pin, ROW8_Pin
};

static GPIO_TypeDef* matrix_cols_port[MATRIX_COLS] = {
    COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port, COL4_GPIO_Port,
    COL5_GPIO_Port, COL6_GPIO_Port, COL7_GPIO_Port, COL8_GPIO_Port
};

static uint16_t matrix_cols_pin[MATRIX_COLS] = {
    COL1_Pin, COL2_Pin, COL3_Pin, COL4_Pin,
    COL5_Pin, COL6_Pin, COL7_Pin, COL8_Pin
};

static uint8_t read_kbd_io(uint8_t row)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(matrix_rows_port[row], matrix_rows_pin[row]);
    
#if KEYBOARD_PULL == 0
    return (state == GPIO_PIN_SET) ? 1 : 0;
#else
    return (state == GPIO_PIN_RESET) ? 1 : 0;
#endif
}

void matrix_init(void)
{
    // Columns are already configured as outputs in MX_GPIO_Init
    // Rows are already configured as inputs with pulldown in MX_GPIO_Init
    
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
        matrix_prev[i] = 0;
    }
    
    keyboard_debouncing.deing = false;
    keyboard_debouncing.de_time = 0;
    
    HAL_Delay(500);
}

static uint8_t matrix_scan(void)
{
    uint8_t data;
    
    for (int col = 0; col < MATRIX_COLS; col++) {
        data = 0;
        
#if KEYBOARD_PULL == 1
        HAL_GPIO_WritePin(matrix_cols_port[col], matrix_cols_pin[col], GPIO_PIN_RESET);
#else
        HAL_GPIO_WritePin(matrix_cols_port[col], matrix_cols_pin[col], GPIO_PIN_SET);
#endif
        
        // Small delay for signal stabilization
        HAL_Delay(1);
        
        data = (
            (read_kbd_io(0) << 0) |
            (read_kbd_io(1) << 1) |
            (read_kbd_io(2) << 2) |
            (read_kbd_io(3) << 3) |
            (read_kbd_io(4) << 4) |
            (read_kbd_io(5) << 5) |
            (read_kbd_io(6) << 6) |
            (read_kbd_io(7) << 7)
        );
        
#if KEYBOARD_PULL == 1
        HAL_GPIO_WritePin(matrix_cols_port[col], matrix_cols_pin[col], GPIO_PIN_SET);
#else
        HAL_GPIO_WritePin(matrix_cols_port[col], matrix_cols_pin[col], GPIO_PIN_RESET);
#endif
        
        if (matrix_debouncing[col] != data) {
            matrix_debouncing[col] = data;
            keyboard_debouncing.deing = true;
            keyboard_debouncing.de_time = HAL_GetTick();
        }
    }
    
    if (keyboard_debouncing.deing == true && 
        ((HAL_GetTick() - keyboard_debouncing.de_time) > DEBOUNCE)) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            matrix[row] = 0;
            for (int col = 0; col < MATRIX_COLS; col++) {
                matrix[row] |= ((matrix_debouncing[col] & (1 << row) ? 1 : 0) << col);
            }
        }
        keyboard_debouncing.deing = false;
    }
    
    return 1;
}

static bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1 << col));
}

static uint8_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

static void matrix_press(uint8_t row, uint8_t col)
{
    if (matrix_is_on(row, col) == true) {
        matrix_action(row, col, KEY_PRESSED);
    }
}

static void matrix_release(uint8_t row, uint8_t col)
{
    if (matrix_is_on(row, col) == false) {
        matrix_action(row, col, KEY_RELEASED);
    }
}

void matrix_task(void)
{
    uint8_t matrix_row = 0;
    uint8_t matrix_change = 0;
    uint8_t pressed = 0;
    
    matrix_scan();
    
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        matrix_row = matrix_get_row(r);
        matrix_change = matrix_row ^ matrix_prev[r];
        if (matrix_change) {
            uint8_t col_mask = 1;
            for (uint8_t c = 0; c < MATRIX_COLS; c++, col_mask <<= 1) {
                if (matrix_change & col_mask) {
                    pressed = (matrix_row & col_mask);
                    if (pressed != 0) {
                        matrix_press(r, c);
                    } else {
                        matrix_release(r, c);
                    }
                    matrix_prev[r] ^= col_mask;
                    keyboard_state.last_activity_time = HAL_GetTick();
                }
            }
        }
    }
}
