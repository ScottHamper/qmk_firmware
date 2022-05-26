#include QMK_KEYBOARD_H

// I'm following example 6 at https://github.com/qmk/qmk_firmware/blob/master/docs/feature_tap_dance.md
// to make a custom tap dance key that will toggle one layer when tapped (Dvorak -> QWERTY), and
// momentarily enable another (LED controls) when held.
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

enum {
    LDANCE,
};

td_state_t cur_dance(qk_tap_dance_state_t *state);
void ldance_finished(qk_tap_dance_state_t *state, void *user_data);
void ldance_reset(qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_75_ansi(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,    KC_F5,    KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,     KC_F11,  KC_F12,  KC_PSCR, KC_INS,  KC_DEL,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,     KC_5,     KC_6,    KC_7,    KC_8,    KC_9,    KC_0,       KC_LBRC, KC_RBRC,          KC_BSPC, KC_HOME,
        KC_TAB,           KC_QUOT, KC_COMM, KC_DOT,   KC_P,     KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,       KC_L,    KC_SLSH, KC_EQL,  KC_BSLS, KC_END,
        KC_CAPS,          KC_A,    KC_O,    KC_E,     KC_U,     KC_I,    KC_D,    KC_H,    KC_T,    KC_N,       KC_S,    KC_MINS, KC_ENT,           KC_PGUP,
        KC_LSFT,          KC_SCLN, KC_Q,    KC_J,     KC_K,     KC_X,    KC_B,    KC_M,    KC_W,    KC_V,       KC_Z,             KC_RSFT, KC_UP,   KC_PGDN,
        KC_LCTL, KC_LGUI,          KC_LALT,                     KC_SPC,                             TD(LDANCE), KC_RGUI, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT),
	[1] = LAYOUT_75_ansi(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_MINS, KC_EQL,           KC_TRNS, KC_TRNS,
        KC_TRNS,          KC_Q,    KC_W,    KC_E,     KC_R,     KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,       KC_P,    KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS,
        KC_TRNS,          KC_A,    KC_S,    KC_D,     KC_F,     KC_G,    KC_H,    KC_J,    KC_K,    KC_L,       KC_SCLN, KC_QUOT, KC_TRNS,          KC_TRNS,
        KC_TRNS,          KC_Z,    KC_X,    KC_C,     KC_V,     KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,     KC_SLSH,          KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,          KC_TRNS,                     KC_TRNS,                            KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
	[2] = LAYOUT_75_ansi(
        RGB_TOG, KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, RGB_M_T, KC_NO,      KC_NO,   KC_NO,            KC_NO,   KC_NO,
        KC_NO,            RGB_HUD, RGB_HUI, KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,            RGB_SAD, RGB_SAI, KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_NO,            KC_NO,
        KC_NO,            RGB_SPD, RGB_SPI, KC_NO,    KC_NO,    KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,            KC_TRNS, RGB_VAI, KC_NO,
        KC_NO,   KC_NO,            KC_NO,                       KC_NO,                              KC_TRNS,    KC_NO,   KC_NO,   KC_NO,   RGB_VAD, KC_NO)
};

td_state_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    }
    else return TD_UNKNOWN;
}

static td_tap_t ldance_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void ldance_finished(qk_tap_dance_state_t *state, void *user_data) {
    ldance_tap_state.state = cur_dance(state);
    switch (ldance_tap_state.state) {
        case TD_SINGLE_TAP:
            if (layer_state_is(0)) {
                layer_on(1);
            } else {
                layer_off(1);
            }
            break;
        case TD_SINGLE_HOLD:
            layer_on(2);
            break;
        default:
            break;
    }
}

void ldance_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (ldance_tap_state.state == TD_SINGLE_HOLD) {
        layer_off(2);
    }
    ldance_tap_state.state = TD_NONE;
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [LDANCE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ldance_finished, ldance_reset)
};
