/*
 * Build-time keymap variants for the Pipar keyboards.
 *
 * Select them with DTS preprocessor flags, for example:
 *   west build ... -- -DDTS_EXTRA_CPPFLAGS="-DPIPAR_LAYOUT_QWERTY;-DPIPAR_OS_MAC"
 *
 * Layout:   PIPAR_LAYOUT_COLEMAK_DH (default) or PIPAR_LAYOUT_QWERTY.
 *           The firmware does the Colemak-DH / QWERTY mapping.
 * Computer: PIPAR_HOST_INTERNATIONAL (default) or PIPAR_HOST_ESTONIAN, the
 *           keyboard layout selected on the computer:
 *             international  US or US International. The Point's Estonian
 *                            keys type through the OS's Unicode input.
 *             estonian       Estonian. Every key is sent as the Estonian key
 *                            that types the same character, so the cheat
 *                            sheets apply to both.
 * OS:       PIPAR_OS_WIN (default), PIPAR_OS_MAC or PIPAR_OS_LINUX.
 *
 * Keymaps using this header must define the hml/hmr home-row-mod behaviours
 * and include <pipar/estonian.dtsi> after defining KEYS_LEFT/KEYS_THUMBS.
 */
#pragma once

#include <pipar/keys_et.h>

/*
 * Alpha block, 3 rows x 10 columns, left to right:
 *   PIPAR_T0..T8 top row, PIPAR_H0..H8 home row, PIPAR_B0..B6 bottom row
 *   are keycodes.
 * The slots that can hold punctuation are complete bindings, because on the
 * Estonian computer layout punctuation needs its own behaviours:
 *   PIPAR_BASE_T9, PIPAR_BASE_H9, PIPAR_BASE_B7..B9 (base layer)
 *   PIPAR_GAME_B8 (gaming layer, no home-row mod; the other slots are shared)
 * On QWERTY, ' takes the right home-row pinky key; ; is on the Num layer.
 */
#if defined(PIPAR_LAYOUT_QWERTY)
#define PIPAR_GAMING_NAME "Gaming"
#define PIPAR_T0 Q
#define PIPAR_T1 W
#define PIPAR_T2 E
#define PIPAR_T3 R
#define PIPAR_T4 T
#define PIPAR_T5 Y
#define PIPAR_T6 U
#define PIPAR_T7 I
#define PIPAR_T8 O
#define PIPAR_BASE_T9 &kp P
#define PIPAR_H0 A
#define PIPAR_H1 S
#define PIPAR_H2 D
#define PIPAR_H3 F
#define PIPAR_H4 G
#define PIPAR_H5 H
#define PIPAR_H6 J
#define PIPAR_H7 K
#define PIPAR_H8 L
#define PIPAR_BASE_H9 PIPAR_HMR_SQT(RGUI)
#define PIPAR_B0 Z
#define PIPAR_B1 X
#define PIPAR_B2 C
#define PIPAR_B3 V
#define PIPAR_B4 B
#define PIPAR_B5 N
#define PIPAR_B6 M
#else /* Colemak-DH */
#define PIPAR_GAMING_NAME "Gaming-CDH"
#define PIPAR_T0 Q
#define PIPAR_T1 W
#define PIPAR_T2 F
#define PIPAR_T3 P
#define PIPAR_T4 B
#define PIPAR_T5 J
#define PIPAR_T6 L
#define PIPAR_T7 U
#define PIPAR_T8 Y
#define PIPAR_BASE_T9 P_SQT
#define PIPAR_H0 A
#define PIPAR_H1 R
#define PIPAR_H2 S
#define PIPAR_H3 T
#define PIPAR_H4 G
#define PIPAR_H5 M
#define PIPAR_H6 N
#define PIPAR_H7 E
#define PIPAR_H8 I
#define PIPAR_BASE_H9 &hmr RGUI O
#define PIPAR_B0 Z
#define PIPAR_B1 X
#define PIPAR_B2 C
#define PIPAR_B3 D
#define PIPAR_B4 V
#define PIPAR_B5 K
#define PIPAR_B6 H
#endif
#define PIPAR_BASE_B7 P_COMMA
#define PIPAR_BASE_B8 PIPAR_HMR_DOT(LALT)
#define PIPAR_GAME_B8 P_DOT
#define PIPAR_BASE_B9 P_SLASH

/*
 * Symbols, as complete bindings: P_<ZMK keycode name>.
 *
 * International: the plain ZMK keycode.
 * Estonian: the Estonian key for the same character (keys_et.h). ' , . /
 * keep their US shifted characters (" < > ?) through mod-morphs. ~ ` ^ are
 * dead keys on some systems; those are sent as the dead key and a space.
 * Apple's Estonian layout has no ` or ^, so they do nothing on macOS.
 *
 * The Estonian Mac variant assumes the keyboard type is set to ISO
 * (European); with ANSI, < > | and ˇ swap places.
 */
#if defined(PIPAR_HOST_ESTONIAN)
#define P_SQT    &et_sqt
#define P_COMMA  &et_comma
#define P_DOT    &et_dot
#define P_SLASH  &et_slash
#define PIPAR_HMR_SQT(mod) &hmr_sqt mod 0
#define PIPAR_HMR_DOT(mod) &hmr_dot mod 0

#define P_LBKT   &kp ET_LBKT
#define P_RBKT   &kp ET_RBKT
#define P_LBRC   &kp ET_LBRC
#define P_RBRC   &kp ET_RBRC
#define P_SEMI   &kp ET_SEMI
#define P_COLON  &kp ET_COLON
#define P_EQUAL  &kp ET_EQUAL
#define P_PLUS   &kp ET_PLUS
#define P_MINUS  &kp ET_MINUS
#define P_UNDER  &kp ET_UNDER
#define P_BSLH   &kp ET_BSLH
#define P_PIPE   &kp ET_PIPE
#define P_AMPS   &kp ET_AMPS
#define P_ASTRK  &kp ET_ASTRK
#define P_LPAR   &kp ET_LPAR
#define P_RPAR   &kp ET_RPAR
#define P_DLLR   &kp ET_DLLR
#define P_PRCNT  &kp ET_PRCNT
#define P_EXCL   &kp ET_EXCL
#define P_AT     &kp ET_AT
#define P_HASH   &kp ET_HASH
#define P_TILDE  &et_tilde
#if defined(PIPAR_OS_MAC)
#define PIPAR_ET_DEAD_TILDE RA(LS(RBKT))  /* Option+Shift+Õ */
#define P_GRAVE  &none
#define P_CARET  &none
#elif defined(PIPAR_OS_LINUX)
#define PIPAR_ET_DEAD_TILDE ET_TILDE      /* Shift+ˇ */
#define P_GRAVE  &kp RA(EQUAL)            /* AltGr+´ */
#define P_CARET  &kp RA(SQT)              /* AltGr+Ä */
#else /* Windows */
#define PIPAR_ET_DEAD_TILDE ET_TILDE      /* Shift+ˇ */
#define PIPAR_ET_DEAD_GRAVE ET_GRAVE      /* Shift+´ */
#define P_GRAVE  &et_grave
#define P_CARET  &kp RA(LS(SQT))          /* AltGr+Shift+Ä */
#endif

#else /* international */
#define P_SQT    &kp SQT
#define P_COMMA  &kp COMMA
#define P_DOT    &kp DOT
#define P_SLASH  &kp SLASH
#define PIPAR_HMR_SQT(mod) &hmr mod SQT
#define PIPAR_HMR_DOT(mod) &hmr mod DOT

#define P_LBKT   &kp LBKT
#define P_RBKT   &kp RBKT
#define P_LBRC   &kp LBRC
#define P_RBRC   &kp RBRC
#define P_SEMI   &kp SEMI
#define P_COLON  &kp COLON
#define P_EQUAL  &kp EQUAL
#define P_PLUS   &kp PLUS
#define P_MINUS  &kp MINUS
#define P_UNDER  &kp UNDER
#define P_BSLH   &kp BSLH
#define P_PIPE   &kp PIPE
#define P_AMPS   &kp AMPS
#define P_ASTRK  &kp ASTRK
#define P_LPAR   &kp LPAR
#define P_RPAR   &kp RPAR
#define P_DLLR   &kp DLLR
#define P_PRCNT  &kp PRCNT
#define P_EXCL   &kp EXCL
#define P_AT     &kp AT
#define P_HASH   &kp HASH
#define P_TILDE  &kp TILDE
#define P_GRAVE  &kp GRAVE
#define P_CARET  &kp CARET
#endif

/*
 * Õ Ä Ö Ü: the Pipar Point's outer pinky keys and the Sool's Nav layer.
 * International: Unicode input (see PIPAR_UC_MODE), lower/upper case code
 * points. Estonian: the Estonian layout's own keys, so Shift and Caps Lock
 * work as usual.
 */
#define ET_OT  0xf5 0xd5    /* õ/Õ */
#define ET_AE  0xe4 0xc4    /* ä/Ä */
#define ET_OE  0xf6 0xd6    /* ö/Ö */
#define ET_UE  0xfc 0xdc    /* ü/Ü */

#if defined(PIPAR_HOST_ESTONIAN)
#define PIPAR_KEY_OT &kp ET_O_TILDE
#define PIPAR_KEY_AE &kp ET_A_UMLAUT
#define PIPAR_KEY_OE &kp ET_O_UMLAUT
#define PIPAR_KEY_UE &kp ET_U_UMLAUT
#else
#define PIPAR_KEY_OT &uc ET_OT
#define PIPAR_KEY_AE &uc ET_AE
#define PIPAR_KEY_OE &uc ET_OE
#define PIPAR_KEY_UE &uc ET_UE
#endif

/*
 * OS-specific parts:
 *   PIPAR_UC_MODE      Unicode input mode for keys that send characters
 *                      directly (the Õ Ä Ö Ü keys on the international
 *                      variant).
 *   PIPAR_UC_MIN_LENGTH  Zero-pad the hex code to this many digits.
 *   PIPAR_CUT/COPY/PASTE  Clipboard shortcuts.
 */
#if defined(PIPAR_OS_MAC)
#define PIPAR_UC_MODE UC_MODE_MACOS   /* needs the "Unicode Hex Input" input source */
#define PIPAR_UC_MIN_LENGTH 4         /* Unicode Hex Input reads 4 hex digits */
#define PIPAR_CUT   LG(X)
#define PIPAR_COPY  LG(C)
#define PIPAR_PASTE LG(V)
#elif defined(PIPAR_OS_LINUX)
#define PIPAR_UC_MODE UC_MODE_LINUX   /* Ctrl+Shift+U, IBus/GTK */
#define PIPAR_UC_MIN_LENGTH 0
#define PIPAR_CUT   LS(DEL)
#define PIPAR_COPY  LC(INS)
#define PIPAR_PASTE LS(INS)
#else /* Windows */
#define PIPAR_UC_MODE UC_MODE_WIN_COMPOSE /* needs WinCompose */
#define PIPAR_UC_MIN_LENGTH 0
#define PIPAR_CUT   LS(DEL)
#define PIPAR_COPY  LC(INS)
#define PIPAR_PASTE LS(INS)
#endif
