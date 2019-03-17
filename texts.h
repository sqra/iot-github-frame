// clearing display
uint8_t clearMe[] = {
  0, 0, 0, 0
};

// displays "IP" text
uint8_t ipText[] = {
  SEG_B | SEG_C,
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G
};

// displays "HI" text
uint8_t hiText[] = {
  SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
  SEG_B | SEG_C
};

// displays "SEt" text
uint8_t setText[] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_D | SEG_E | SEG_F | SEG_G
};

// displays "rEPO" text
uint8_t repoText[] = {
    SEG_E | SEG_G,
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
};

// TIP:
// Each of the four digits has 7 segments that can be addressed each individually. 
// Each segment is represented by a letter as shown below which is addressed by one bit.
//
//      A
//     ----
// F |     | B
//     -G-
// E |     | C
//     ----
//      D
