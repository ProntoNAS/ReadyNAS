/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -m 10 ./unictype/scripts_byname.gperf  */
/* Computed positions: -k'1,3,5' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 4 "./unictype/scripts_byname.gperf"
struct named_script { int name; unsigned int index; };

#define TOTAL_KEYWORDS 95
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 22
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 121
/* maximum key range = 120, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
scripts_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122,  48,   4,  10,  48,  30,
      122,  28,  35,  19,  52,  26,  20,  46,  64,  11,
       37, 122,  52,   8,   0,   3,  70, 122, 122,   0,
      122, 122, 122, 122, 122,  56, 122,   0,  34,  44,
        8,  11,   0,   1,  19,  12,  25,  21,   0,   8,
        4,  22,  11, 122,   8,  29,  42,  31,  31,  16,
      122,  29, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122, 122, 122, 122, 122,
      122, 122, 122, 122, 122, 122
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct script_stringpool_t
  {
    char script_stringpool_str2[sizeof("Yi")];
    char script_stringpool_str4[sizeof("Thai")];
    char script_stringpool_str7[sizeof("Telugu")];
    char script_stringpool_str8[sizeof("Tagalog")];
    char script_stringpool_str9[sizeof("Tagbanwa")];
    char script_stringpool_str10[sizeof("Thaana")];
    char script_stringpool_str11[sizeof("Braille")];
    char script_stringpool_str12[sizeof("Tifinagh")];
    char script_stringpool_str13[sizeof("Tamil")];
    char script_stringpool_str14[sizeof("Cham")];
    char script_stringpool_str15[sizeof("Bengali")];
    char script_stringpool_str16[sizeof("Balinese")];
    char script_stringpool_str17[sizeof("Buginese")];
    char script_stringpool_str18[sizeof("Brahmi")];
    char script_stringpool_str19[sizeof("Sinhala")];
    char script_stringpool_str20[sizeof("Tai_Tham")];
    char script_stringpool_str21[sizeof("Sundanese")];
    char script_stringpool_str22[sizeof("Syriac")];
    char script_stringpool_str23[sizeof("Ugaritic")];
    char script_stringpool_str24[sizeof("Carian")];
    char script_stringpool_str25[sizeof("Bamum")];
    char script_stringpool_str26[sizeof("Cyrillic")];
    char script_stringpool_str27[sizeof("Shavian")];
    char script_stringpool_str28[sizeof("Oriya")];
    char script_stringpool_str29[sizeof("Old_Turkic")];
    char script_stringpool_str30[sizeof("Osmanya")];
    char script_stringpool_str31[sizeof("Bopomofo")];
    char script_stringpool_str32[sizeof("Linear_B")];
    char script_stringpool_str33[sizeof("Samaritan")];
    char script_stringpool_str34[sizeof("Lydian")];
    char script_stringpool_str35[sizeof("Cuneiform")];
    char script_stringpool_str36[sizeof("Buhid")];
    char script_stringpool_str37[sizeof("Kannada")];
    char script_stringpool_str38[sizeof("Tai_Le")];
    char script_stringpool_str39[sizeof("Coptic")];
    char script_stringpool_str40[sizeof("Cypriot")];
    char script_stringpool_str41[sizeof("Canadian_Aboriginal")];
    char script_stringpool_str42[sizeof("Han")];
    char script_stringpool_str43[sizeof("Ogham")];
    char script_stringpool_str44[sizeof("Old_South_Arabian")];
    char script_stringpool_str45[sizeof("Lao")];
    char script_stringpool_str46[sizeof("Common")];
    char script_stringpool_str47[sizeof("Khmer")];
    char script_stringpool_str48[sizeof("Old_Italic")];
    char script_stringpool_str49[sizeof("Saurashtra")];
    char script_stringpool_str50[sizeof("Hanunoo")];
    char script_stringpool_str51[sizeof("Cherokee")];
    char script_stringpool_str52[sizeof("Hiragana")];
    char script_stringpool_str53[sizeof("Lisu")];
    char script_stringpool_str54[sizeof("Imperial_Aramaic")];
    char script_stringpool_str55[sizeof("Inherited")];
    char script_stringpool_str56[sizeof("Lepcha")];
    char script_stringpool_str57[sizeof("Mandaic")];
    char script_stringpool_str58[sizeof("Kharoshthi")];
    char script_stringpool_str59[sizeof("Georgian")];
    char script_stringpool_str60[sizeof("Glagolitic")];
    char script_stringpool_str61[sizeof("Myanmar")];
    char script_stringpool_str62[sizeof("Syloti_Nagri")];
    char script_stringpool_str63[sizeof("Kaithi")];
    char script_stringpool_str64[sizeof("Limbu")];
    char script_stringpool_str65[sizeof("Greek")];
    char script_stringpool_str66[sizeof("Arabic")];
    char script_stringpool_str67[sizeof("Old_Persian")];
    char script_stringpool_str68[sizeof("Armenian")];
    char script_stringpool_str69[sizeof("Gujarati")];
    char script_stringpool_str70[sizeof("Lycian")];
    char script_stringpool_str71[sizeof("Latin")];
    char script_stringpool_str72[sizeof("Batak")];
    char script_stringpool_str73[sizeof("Phoenician")];
    char script_stringpool_str74[sizeof("Phags_Pa")];
    char script_stringpool_str75[sizeof("Gurmukhi")];
    char script_stringpool_str76[sizeof("Hangul")];
    char script_stringpool_str77[sizeof("Inscriptional_Pahlavi")];
    char script_stringpool_str78[sizeof("Inscriptional_Parthian")];
    char script_stringpool_str79[sizeof("Ethiopic")];
    char script_stringpool_str80[sizeof("Meetei_Mayek")];
    char script_stringpool_str81[sizeof("Mongolian")];
    char script_stringpool_str82[sizeof("Kayah_Li")];
    char script_stringpool_str83[sizeof("Tibetan")];
    char script_stringpool_str84[sizeof("Malayalam")];
    char script_stringpool_str85[sizeof("Vai")];
    char script_stringpool_str86[sizeof("Hebrew")];
    char script_stringpool_str87[sizeof("Rejang")];
    char script_stringpool_str88[sizeof("Gothic")];
    char script_stringpool_str89[sizeof("Nko")];
    char script_stringpool_str90[sizeof("Tai_Viet")];
    char script_stringpool_str91[sizeof("New_Tai_Lue")];
    char script_stringpool_str92[sizeof("Deseret")];
    char script_stringpool_str93[sizeof("Devanagari")];
    char script_stringpool_str94[sizeof("Ol_Chiki")];
    char script_stringpool_str95[sizeof("Javanese")];
    char script_stringpool_str97[sizeof("Katakana")];
    char script_stringpool_str105[sizeof("Runic")];
    char script_stringpool_str108[sizeof("Avestan")];
    char script_stringpool_str121[sizeof("Egyptian_Hieroglyphs")];
  };
static const struct script_stringpool_t script_stringpool_contents =
  {
    "Yi",
    "Thai",
    "Telugu",
    "Tagalog",
    "Tagbanwa",
    "Thaana",
    "Braille",
    "Tifinagh",
    "Tamil",
    "Cham",
    "Bengali",
    "Balinese",
    "Buginese",
    "Brahmi",
    "Sinhala",
    "Tai_Tham",
    "Sundanese",
    "Syriac",
    "Ugaritic",
    "Carian",
    "Bamum",
    "Cyrillic",
    "Shavian",
    "Oriya",
    "Old_Turkic",
    "Osmanya",
    "Bopomofo",
    "Linear_B",
    "Samaritan",
    "Lydian",
    "Cuneiform",
    "Buhid",
    "Kannada",
    "Tai_Le",
    "Coptic",
    "Cypriot",
    "Canadian_Aboriginal",
    "Han",
    "Ogham",
    "Old_South_Arabian",
    "Lao",
    "Common",
    "Khmer",
    "Old_Italic",
    "Saurashtra",
    "Hanunoo",
    "Cherokee",
    "Hiragana",
    "Lisu",
    "Imperial_Aramaic",
    "Inherited",
    "Lepcha",
    "Mandaic",
    "Kharoshthi",
    "Georgian",
    "Glagolitic",
    "Myanmar",
    "Syloti_Nagri",
    "Kaithi",
    "Limbu",
    "Greek",
    "Arabic",
    "Old_Persian",
    "Armenian",
    "Gujarati",
    "Lycian",
    "Latin",
    "Batak",
    "Phoenician",
    "Phags_Pa",
    "Gurmukhi",
    "Hangul",
    "Inscriptional_Pahlavi",
    "Inscriptional_Parthian",
    "Ethiopic",
    "Meetei_Mayek",
    "Mongolian",
    "Kayah_Li",
    "Tibetan",
    "Malayalam",
    "Vai",
    "Hebrew",
    "Rejang",
    "Gothic",
    "Nko",
    "Tai_Viet",
    "New_Tai_Lue",
    "Deseret",
    "Devanagari",
    "Ol_Chiki",
    "Javanese",
    "Katakana",
    "Runic",
    "Avestan",
    "Egyptian_Hieroglyphs"
  };
#define script_stringpool ((const char *) &script_stringpool_contents)

static const struct named_script script_names[] =
  {
    {-1}, {-1},
#line 51 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str2, 36},
    {-1},
#line 34 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str4, 19},
    {-1}, {-1},
#line 30 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str7, 15},
#line 56 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str8, 41},
#line 59 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str9, 44},
#line 23 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str10, 8},
#line 67 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str11, 52},
#line 72 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str12, 57},
#line 29 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str13, 14},
#line 91 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str14, 76},
#line 25 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str15, 10},
#line 76 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str16, 61},
#line 68 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str17, 53},
#line 108 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str18, 93},
#line 33 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str19, 18},
#line 92 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str20, 77},
#line 81 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str21, 66},
#line 22 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str22, 7},
#line 63 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str23, 48},
#line 89 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str24, 74},
#line 98 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str25, 83},
#line 18 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str26, 3},
#line 64 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str27, 49},
#line 28 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str28, 13},
#line 105 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str29, 90},
#line 65 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str30, 50},
#line 49 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str31, 34},
#line 62 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str32, 47},
#line 96 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str33, 81},
#line 90 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str34, 75},
#line 77 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str35, 62},
#line 58 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str36, 43},
#line 31 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str37, 16},
#line 61 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str38, 46},
#line 69 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str39, 54},
#line 66 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str40, 51},
#line 42 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str41, 27},
#line 50 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str42, 35},
#line 43 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str43, 28},
#line 102 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str44, 87},
#line 35 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str45, 20},
#line 15 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str46, 0},
#line 45 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str47, 30},
#line 52 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str48, 37},
#line 85 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str49, 70},
#line 57 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str50, 42},
#line 41 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str51, 26},
#line 47 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str52, 32},
#line 97 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str53, 82},
#line 101 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str54, 86},
#line 55 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str55, 40},
#line 82 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str56, 67},
#line 109 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str57, 94},
#line 75 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str58, 60},
#line 38 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str59, 23},
#line 71 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str60, 56},
#line 37 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str61, 22},
#line 73 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str62, 58},
#line 106 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str63, 91},
#line 60 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str64, 45},
#line 17 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str65, 2},
#line 21 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str66, 6},
#line 74 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str67, 59},
#line 19 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str68, 4},
#line 27 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str69, 12},
#line 88 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str70, 73},
#line 16 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str71, 1},
#line 107 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str72, 92},
#line 78 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str73, 63},
#line 79 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str74, 64},
#line 26 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str75, 11},
#line 39 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str76, 24},
#line 104 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str77, 89},
#line 103 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str78, 88},
#line 40 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str79, 25},
#line 100 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str80, 85},
#line 46 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str81, 31},
#line 86 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str82, 71},
#line 36 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str83, 21},
#line 32 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str84, 17},
#line 84 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str85, 69},
#line 20 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str86, 5},
#line 87 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str87, 72},
#line 53 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str88, 38},
#line 80 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str89, 65},
#line 93 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str90, 78},
#line 70 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str91, 55},
#line 54 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str92, 39},
#line 24 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str93, 9},
#line 83 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str94, 68},
#line 99 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str95, 84},
    {-1},
#line 48 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str97, 33},
    {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 44 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str105, 29},
    {-1}, {-1},
#line 94 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str108, 79},
    {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
    {-1}, {-1}, {-1},
#line 95 "./unictype/scripts_byname.gperf"
    {(int)(long)&((struct script_stringpool_t *)0)->script_stringpool_str121, 80}
  };

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct named_script *
uc_script_lookup (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = scripts_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = script_names[key].name;
          if (o >= 0)
            {
              register const char *s = o + script_stringpool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &script_names[key];
            }
        }
    }
  return 0;
}
