/*
 * Build-time keymap variants for the Pipar keyboards.
 *
 * Select them with DTS preprocessor flags, for example:
 *   west build ... -- -DDTS_EXTRA_CPPFLAGS="-DPIPAR_LAYOUT_QWERTY;-DPIPAR_OS_MAC"
 *
 * Layout: PIPAR_LAYOUT_COLEMAK_DH (default) or PIPAR_LAYOUT_QWERTY.
 * OS:     PIPAR_OS_WIN (default), PIPAR_OS_MAC or PIPAR_OS_LINUX.
 *
 * The computer should stay on a US layout (US International for Estonian
 * letters); the firmware does the Colemak-DH / QWERTY mapping.
 */
#pragma once

/*
 * Alpha block, 3 rows x 10 columns, left to right:
 *   PIPAR_T0..T9 top row, PIPAR_H0..H9 home row, PIPAR_B0..B9 bottom row.
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
#define PIPAR_T9 P
#define PIPAR_H0 A
#define PIPAR_H1 S
#define PIPAR_H2 D
#define PIPAR_H3 F
#define PIPAR_H4 G
#define PIPAR_H5 H
#define PIPAR_H6 J
#define PIPAR_H7 K
#define PIPAR_H8 L
#define PIPAR_H9 SQT
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
#define PIPAR_T9 SQT
#define PIPAR_H0 A
#define PIPAR_H1 R
#define PIPAR_H2 S
#define PIPAR_H3 T
#define PIPAR_H4 G
#define PIPAR_H5 M
#define PIPAR_H6 N
#define PIPAR_H7 E
#define PIPAR_H8 I
#define PIPAR_H9 O
#define PIPAR_B0 Z
#define PIPAR_B1 X
#define PIPAR_B2 C
#define PIPAR_B3 D
#define PIPAR_B4 V
#define PIPAR_B5 K
#define PIPAR_B6 H
#endif
#define PIPAR_B7 COMMA
#define PIPAR_B8 DOT
#define PIPAR_B9 SLASH

/*
 * OS-specific parts:
 *   PIPAR_UC_MODE      Unicode input mode for keys that send characters
 *                      directly (the Pipar Point's Estonian keys).
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
