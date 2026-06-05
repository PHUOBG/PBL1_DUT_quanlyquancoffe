/*
 * ITF COFFEE — Hệ Thống Quản Lý Quán Cà Phê
 * Giao diện đồ họa Raylib — Full Vietnamese Support
 *
 * BIÊN DỊCH:
 *   Linux/macOS:
 *     gcc gcoffee_raylib.c -o gcoffee -lraylib -lm
 *
 *   Windows (MinGW + Raylib đã cài):
 *     gcc gcoffee_raylib.c -o gcoffee.exe -lraylib -lopengl32 -lgdi32 -lwinmm
 * -mwindows
 */

#include "raylib.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ═══════════════════════════════════════════════════════════════════
   HẰNG SỐ
═══════════════════════════════════════════════════════════════════ */
#define MAX_STAFF 100
#define MAX_MENU 100
#define MAX_TABLES 10
#define MAX_ITEMS 50
#define STR_LEN 50
#define SHORT_STR 30

#define WIN_W 1280
#define WIN_H 800
#define WW GetScreenWidth()
#define WH GetScreenHeight()
#define SIDEBAR_W 300
#define TOPBAR_H 80
#define FPS 60

#define MIN3(a, b, c)                                                          \
  ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/* ═══════════════════════════════════════════════════════════════════
   CẤU TRÚC DỮ LIỆU
═══════════════════════════════════════════════════════════════════ */
typedef struct {
  int id;
  char username[SHORT_STR];
  char password[SHORT_STR];
  char name[STR_LEN];
  char position[SHORT_STR];
  float salaryPerHour;
  float totalSalaryEarned;
  int status;
  long lastCheckIn;
} Staff;

typedef struct {
  int id;
  char name[STR_LEN];
  float price;
  int inStock;
} MenuItem;

typedef struct {
  int menuId;
  char name[STR_LEN];
  int qty;
  float price;
} OrderItem;

typedef struct {
  int id;
  int isOccupied;
  OrderItem items[MAX_ITEMS];
  int itemCount;
  float currentBill;
  char staffName[STR_LEN];
} Table;

/* ═══════════════════════════════════════════════════════════════════
   DỮ LIỆU TOÀN CỤC
═══════════════════════════════════════════════════════════════════ */
static Staff gStaff[MAX_STAFF];
static int gStaffCount = 0;
static MenuItem gMenu[MAX_MENU];
static int gMenuCount = 0;
static Table gTables[MAX_TABLES];
static float gTotalRevenue = 0;
static Staff *gCurrentStaff = NULL;

/* ═══════════════════════════════════════
   WHITE • BLUE • PINK THEME
═══════════════════════════════════════ */
/* Background */
#define CB_BG CLITERAL(Color){245, 248, 255, 255}
#define CB_PANEL CLITERAL(Color){255, 255, 255, 255}
#define CB_CARD CLITERAL(Color){238, 244, 255, 255}
#define CB_CARD_HOV CLITERAL(Color){224, 236, 255, 255}
#define CB_ROW_ALT CLITERAL(Color){230, 238, 252, 255}
#define CB_INPUT CLITERAL(Color){250, 252, 255, 255}

/* Sidebar / Topbar */
#define CB_SIDEBAR CLITERAL(Color){219, 233, 255, 255}
#define CB_TOPBAR CLITERAL(Color){255, 255, 255, 255}

/* Borders */
#define CB_BORDER CLITERAL(Color){190, 210, 245, 255}
#define CB_BORDER2 CLITERAL(Color){120, 170, 255, 255}

/* Primary blue */
#define CA_GOLD CLITERAL(Color){70, 140, 255, 255}
#define CA_GOLD_DIM CLITERAL(Color){120, 180, 255, 255}
#define CA_GOLD_LITE CLITERAL(Color){180, 215, 255, 255}

/* Pink accent */
#define CA_PINK CLITERAL(Color){255, 120, 190, 255}
#define CA_PINK_LITE CLITERAL(Color){255, 210, 235, 255}

/* Text */
#define CT_WHITE CLITERAL(Color){35, 45, 65, 255}
#define CT_MUTED CLITERAL(Color){100, 120, 155, 255}
#define CT_DIM CLITERAL(Color){140, 155, 180, 255}

/* Status */
#define CS_OK CLITERAL(Color){60, 180, 120, 255}
#define CS_ERR CLITERAL(Color){255, 90, 120, 255}
#define CS_WARN CLITERAL(Color){255, 180, 70, 255}
#define CS_INFO CLITERAL(Color){80, 150, 255, 255}

/* ═══════════════════════════════════════════════════════════════════
   MÀN HÌNH
═══════════════════════════════════════════════════════════════════ */
typedef enum {
  SCR_LOGIN,
  SCR_TABLES,
  SCR_ORDER,
  SCR_INVOICE,
  SCR_MENU,
  SCR_STAFF,
  SCR_STATS,
  SCR_CHART,
} Screen;

/* ═══════════════════════════════════════════════════════════════════
   TRẠNG THÁI UI TOÀN CỤC
═══════════════════════════════════════════════════════════════════ */
#define MAX_INP 14
#define MAX_HBTN 128

static Font gFont, gFontB;
static float gElapsed = 0;
static Screen gScreen = SCR_LOGIN;

/* Input fields */
static char gInp[MAX_INP][1024];
static int gInpLen[MAX_INP];
static bool gInpPass[MAX_INP];
static int gActiveInp = -1;
static float gCursorBlink = 0;

/* Cuộn */
static float gScrollY = 0, gScrollTarget = 0;

/* Chọn bàn */
static int gSelTable = 0;

/* Sub-màn hình: 0=list, 1=add, 2=edit */
static int gSubScr = 0;
static int gEditId = -1;

/* Toast */
static char gToastMsg[256];
static float gToastTimer = 0;
static Color gToastColor;
static bool gToastActive = false;

/* Dialog */
static bool gDlgOpen = false;
static char gDlgTitle[128];
static char gDlgBody[512];
static int gDlgAction = 0;

/* Hover animation per button slot */
static float gHov[MAX_HBTN];

/* Tìm kiếm */
static char gSearch[256];
static int gSearchLen = 0;

/* ═══════════════════════════════════════════════════════════════════
   TIỆN ÍCH UTF-8
═══════════════════════════════════════════════════════════════════ */
static int utf8Encode(int cp, char *out) {
  if (cp <= 0x7F) {
    out[0] = (char)cp;
    return 1;
  }
  if (cp <= 0x7FF) {
    out[0] = (char)(0xC0 | (cp >> 6));
    out[1] = (char)(0x80 | (cp & 0x3F));
    return 2;
  }
  if (cp <= 0xFFFF) {
    out[0] = (char)(0xE0 | (cp >> 12));
    out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[2] = (char)(0x80 | (cp & 0x3F));
    return 3;
  }
  return 0;
}
static void utf8DeleteLast(char *s, int *len) {
  if (!*len)
    return;
  int i = *len - 1;
  while (i > 0 && ((unsigned char)s[i] & 0xC0) == 0x80)
    i--;
  s[i] = '\0';
  *len = i;
}

/* ═══════════════════════════════════════════════════════════════════
   TIỆN ÍCH ĐO/VẼ CHỮ
═══════════════════════════════════════════════════════════════════ */
static inline Vector2 Measure(const char *s, float sz) {
  return MeasureTextEx(gFont, s, sz, 1.f);
}
static inline Vector2 MeasureB(const char *s, float sz) {
  return MeasureTextEx(gFontB, s, sz, 1.f);
}
static void DrawTxtL(const char *s, float x, float y, float sz, Color c) {
  DrawTextEx(gFont, s, (Vector2){x, y}, sz, 1.f, c);
}
static void DrawTxtBL(const char *s, float x, float y, float sz, Color c) {
  DrawTextEx(gFontB, s, (Vector2){x, y}, sz, 1.f, c);
}
static void DrawTxtCenter(const char *s, Rectangle r, float sz, Color c,
                          bool bold) {
  Vector2 m = bold ? MeasureB(s, sz) : Measure(s, sz);
  float tx = r.x + (r.width - m.x) * 0.5f;
  float ty = r.y + (r.height - m.y) * 0.5f;
  if (bold)
    DrawTextEx(gFontB, s, (Vector2){tx, ty}, sz, 1.f, c);
  else
    DrawTextEx(gFont, s, (Vector2){tx, ty}, sz, 1.f, c);
}
static void DrawTxtVCL(const char *s, Rectangle r, float sz, Color c,
                       float padL, bool bold) {
  Vector2 m = bold ? MeasureB(s, sz) : Measure(s, sz);
  float ty = r.y + (r.height - m.y) * 0.5f;
  if (bold)
    DrawTextEx(gFontB, s, (Vector2){r.x + padL, ty}, sz, 1.f, c);
  else
    DrawTextEx(gFont, s, (Vector2){r.x + padL, ty}, sz, 1.f, c);
}

/* ═══════════════════════════════════════════════════════════════════
   CHUẨN HÓA TIẾNG VIỆT (bỏ dấu, viết thường — để tìm kiếm)
═══════════════════════════════════════════════════════════════════ */
static void normalizeVN(const char *src, char *dst) {
  static const char *U[] = {
      "á", "à", "ả", "ã", "ạ", "ă", "ắ", "ằ", "ẳ", "ẵ", "ặ", "â", "ấ", "ầ", "ẩ",
      "ẫ", "ậ", "Á", "À", "Ả", "Ã", "Ạ", "Ă", "Ắ", "Ằ", "Ẳ", "Ẵ", "Ặ", "Â", "Ấ",
      "Ầ", "Ẩ", "Ẫ", "Ậ", "đ", "Đ", "é", "è", "ẻ", "ẽ", "ẹ", "ê", "ế", "ề", "ể",
      "ễ", "ệ", "É", "È", "Ẻ", "Ẽ", "Ẹ", "Ê", "Ế", "Ề", "Ể", "Ễ", "Ệ", "í", "ì",
      "ỉ", "ĩ", "ị", "Í", "Ì", "Ỉ", "Ĩ", "Ị", "ó", "ò", "ỏ", "õ", "ọ", "ô", "ố",
      "ồ", "ổ", "ỗ", "ộ", "ơ", "ớ", "ờ", "ở", "ỡ", "ợ", "Ó", "Ò", "Ỏ", "Õ", "Ọ",
      "Ô", "Ố", "Ồ", "Ổ", "Ỗ", "Ộ", "Ơ", "Ớ", "Ờ", "Ở", "Ỡ", "Ợ", "ú", "ù", "ủ",
      "ũ", "ụ", "ư", "ứ", "ừ", "ử", "ữ", "ự", "Ú", "Ù", "Ủ", "Ũ", "Ụ", "Ư", "Ứ",
      "Ừ", "Ử", "Ữ", "Ự", "ý", "ỳ", "ỷ", "ỹ", "ỵ", "Ý", "Ỳ", "Ỷ", "Ỹ", "Ỵ"};
  static const char *N[] = {
      "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a",
      "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a", "a",
      "a", "a", "a", "a", "d", "d", "e", "e", "e", "e", "e", "e", "e", "e", "e",
      "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", "e", "i", "i",
      "i", "i", "i", "i", "i", "i", "i", "i", "o", "o", "o", "o", "o", "o", "o",
      "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o",
      "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "u", "u", "u",
      "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u", "u",
      "u", "u", "u", "u", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y"};
  char tmp[2048];
  strncpy(tmp, src, 2047);
  tmp[2047] = 0;
  for (int k = 0; k < 134; k++) {
    char *p;
    int lu = (int)strlen(U[k]), ln = (int)strlen(N[k]);
    while ((p = strstr(tmp, U[k])) != NULL) {
      memmove(p + ln, p + lu, strlen(p + lu) + 1);
      memcpy(p, N[k], ln);
    }
  }
  int i = 0;
  for (; tmp[i]; i++)
    dst[i] = (char)tolower((unsigned char)tmp[i]);
  dst[i] = 0;
}

static int levenshtein(const char *a, const char *b) {
  int la = (int)strlen(a), lb = (int)strlen(b);
  int *col = (int *)malloc((la + 1) * sizeof(int));
  if (!col)
    return 99;
  for (int y = 0; y <= la; y++)
    col[y] = y;
  for (int x = 1; x <= lb; x++) {
    col[0] = x;
    for (int y = 1, ld = x - 1; y <= la; y++) {
      int od = col[y];
      col[y] =
          MIN3(col[y] + 1, col[y - 1] + 1, ld + (a[y - 1] == b[x - 1] ? 0 : 1));
      ld = od;
    }
  }
  int r = col[la];
  free(col);
  return r;
}

static bool matchSearch(const char *query, const char *target) {
  if (!query[0])
    return true;
  char nq[2048], nt[2048];
  normalizeVN(query, nq);
  normalizeVN(target, nt);
  return strstr(nt, nq) != NULL || levenshtein(nq, nt) <= 3;
}

/* ═══════════════════════════════════════════════════════════════════
   LƯU / TẢI DỮ LIỆU
═══════════════════════════════════════════════════════════════════ */
static void saveStaff(void) {
  FILE *fp = fopen("staff.txt", "w");
  if (!fp)
    return;
  fprintf(fp, "%d\n", gStaffCount);
  for (int i = 0; i < gStaffCount; i++)
    fprintf(fp, "%d\n%s\n%s\n%s\n%s\n%.2f\n%.2f\n%d\n%ld\n", gStaff[i].id,
            gStaff[i].username, gStaff[i].password, gStaff[i].name,
            gStaff[i].position, gStaff[i].salaryPerHour,
            gStaff[i].totalSalaryEarned, gStaff[i].status,
            gStaff[i].lastCheckIn);
  fclose(fp);
}
static void loadStaff(void) {
  FILE *fp = fopen("staff.txt", "r");
  if (fp) {
    if (fscanf(fp, "%d\n", &gStaffCount) != 1) {
      fclose(fp);
      return;
    }
    for (int i = 0; i < gStaffCount; i++) {
      fscanf(fp, "%d\n", &gStaff[i].id);
      fgets(gStaff[i].username, SHORT_STR, fp);
      gStaff[i].username[strcspn(gStaff[i].username, "\n")] = 0;
      fgets(gStaff[i].password, SHORT_STR, fp);
      gStaff[i].password[strcspn(gStaff[i].password, "\n")] = 0;
      fgets(gStaff[i].name, STR_LEN, fp);
      gStaff[i].name[strcspn(gStaff[i].name, "\n")] = 0;
      fgets(gStaff[i].position, SHORT_STR, fp);
      gStaff[i].position[strcspn(gStaff[i].position, "\n")] = 0;
      long ci;
      fscanf(fp, "%f\n%f\n%d\n%ld\n", &gStaff[i].salaryPerHour,
             &gStaff[i].totalSalaryEarned, &gStaff[i].status, &ci);
      gStaff[i].lastCheckIn = ci;
    }
    fclose(fp);
  } else {
    memset(&gStaff[0], 0, sizeof(Staff));
    gStaff[0].id = 100;
    strcpy(gStaff[0].username, "admin");
    strcpy(gStaff[0].password, "123456");
    strcpy(gStaff[0].name, "Quản Trị Viên");
    strcpy(gStaff[0].position, "Admin");
    gStaffCount = 1;
    saveStaff();
  }
}
static void saveMenu(void) {
  FILE *fp = fopen("menu.txt", "w");
  if (!fp)
    return;
  fprintf(fp, "%d\n", gMenuCount);
  for (int i = 0; i < gMenuCount; i++)
    fprintf(fp, "%d\n%s\n%.0f\n%d\n", gMenu[i].id, gMenu[i].name,
            gMenu[i].price, gMenu[i].inStock);
  fclose(fp);
}
static void loadMenu(void) {
  FILE *fp = fopen("menu.txt", "r");
  if (fp) {
    if (fscanf(fp, "%d\n", &gMenuCount) != 1) {
      fclose(fp);
      return;
    }
    for (int i = 0; i < gMenuCount; i++) {
      fscanf(fp, "%d\n", &gMenu[i].id);
      fgets(gMenu[i].name, STR_LEN, fp);
      gMenu[i].name[strcspn(gMenu[i].name, "\n")] = 0;
      fscanf(fp, "%f\n%d\n", &gMenu[i].price, &gMenu[i].inStock);
    }
    fclose(fp);
  } else {
    gMenu[0] = (MenuItem){1, "Cà phê đen", 15000, 1};
    gMenu[1] = (MenuItem){2, "Cà phê sữa", 20000, 1};
    gMenu[2] = (MenuItem){3, "Bạc xỉu", 25000, 1};
    gMenu[3] = (MenuItem){4, "Trà đào cam sả", 35000, 0};
    gMenuCount = 4;
    saveMenu();
  }
}
static void saveInvoice(int t) {
  time_t now = time(NULL);
  struct tm *ti = localtime(&now);
  char fn[64];
  sprintf(fn, "hoadon_%02d_%02d_%04d.txt", ti->tm_mday, ti->tm_mon + 1,
          ti->tm_year + 1900);
  FILE *fp = fopen(fn, "a");
  if (fp) {
    fprintf(
        fp,
        "================================================================\n");
    fprintf(fp, "Thời gian: %02d:%02d:%02d\n", ti->tm_hour, ti->tm_min,
            ti->tm_sec);
    fprintf(fp, "Bàn: %d | Thu ngân: %s\n", gTables[t].id,
            gCurrentStaff ? gCurrentStaff->name : "?");
    fprintf(
        fp,
        "----------------------------------------------------------------\n");
    for (int i = 0; i < gTables[t].itemCount; i++)
      fprintf(fp, "%-25s | %-10.0f | %-4d | %.0f VND\n",
              gTables[t].items[i].name, gTables[t].items[i].price,
              gTables[t].items[i].qty,
              gTables[t].items[i].price * gTables[t].items[i].qty);
    fprintf(
        fp,
        "----------------------------------------------------------------\n");
    fprintf(fp, "TỔNG: %.0f VND\n", gTables[t].currentBill);
    fprintf(
        fp,
        "================================================================\n\n");
    fclose(fp);
  }
  FILE *fr = fopen("tongdoanhthu.txt", "a");
  if (fr) {
    fprintf(fr, "%02d/%02d/%04d - Bàn %d - Tổng: %.0f VND\n", ti->tm_mday,
            ti->tm_mon + 1, ti->tm_year + 1900, gTables[t].id,
            gTables[t].currentBill);
    fclose(fr);
  }
}

/* ═══════════════════════════════════════════════════════════════════
   TẢI FONT TIẾNG VIỆT
═══════════════════════════════════════════════════════════════════ */
static Font loadVietFont(const char *path, int size) {
  int cp[700];
  int n = 0;
  /* ASCII */
  for (int i = 32; i < 127; i++)
    cp[n++] = i;
  /* Latin Extended */
  int ext[] = {0xC0,  0xC1,  0xC2,  0xC3,  0xC8,  0xC9,  0xCA,  0xCC,
               0xCD,  0xD2,  0xD3,  0xD4,  0xD5,  0xD9,  0xDA,  0xDD,
               0xE0,  0xE1,  0xE2,  0xE3,  0xE8,  0xE9,  0xEA,  0xEC,
               0xED,  0xF2,  0xF3,  0xF4,  0xF5,  0xF9,  0xFA,  0xFD,
               0x102, 0x103, 0x110, 0x111, 0x1A0, 0x1A1, 0x1AF, 0x1B0};
  for (int i = 0; i < 40; i++)
    cp[n++] = ext[i];
  /* Latin Extended Additional (toàn bộ tiếng Việt) */
  for (int i = 0x1EA0; i <= 0x1EF9; i++)
    cp[n++] = i;
  /* Ký hiệu tiền VND + các dấu */
  int sym[] = {0x20AB, 0x2014, 0x2013, 0x2022, 0x25CF,
               0x2713, 0x2715, 0x00B7, 0x00B0};
  for (int i = 0; i < 9; i++)
    cp[n++] = sym[i];

  Font f = LoadFontEx(path, size, cp, n);
  SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
  return f;
}

static Font tryLoadFont(const char *paths[], int size) {
  for (int i = 0; paths[i]; i++) {
    if (FileExists(paths[i]))
      return loadVietFont(paths[i], size);
  }
  return GetFontDefault();
}

/* ═══════════════════════════════════════════════════════════════════
   QUẢN LÝ MÀN HÌNH & TRẠNG THÁI
═══════════════════════════════════════════════════════════════════ */
static void resetScreen(Screen s) {
  gScreen = s;
  gScrollY = 0;
  gScrollTarget = 0;
  gSubScr = 0;
  gEditId = -1;
  gActiveInp = -1;
  gDlgOpen = false;
  memset(gInp, 0, sizeof(gInp));
  memset(gInpLen, 0, sizeof(gInpLen));
  memset(gInpPass, 0, sizeof(gInpPass));
  gSearch[0] = 0;
  gSearchLen = 0;
}

static void showToast(const char *msg, Color col) {
  strncpy(gToastMsg, msg, 255);
  gToastTimer = 3.0f;
  gToastColor = col;
  gToastActive = true;
}

static void openDialog(const char *title, const char *body, int action) {
  gDlgOpen = true;
  strncpy(gDlgTitle, title, 127);
  strncpy(gDlgBody, body, 511);
  gDlgAction = action;
}

/* ═══════════════════════════════════════════════════════════════════
   TWEEN HELPER
═══════════════════════════════════════════════════════════════════ */
static float lerp(float a, float b, float t) { return a + (b - a) * t; }
static float smoothStep(float from, float to, float speed) {
  return lerp(from, to, 1.f - expf(-speed * GetFrameTime()));
}

static void updateHov(int slot, bool hovered) {
  gHov[slot] = smoothStep(gHov[slot], hovered ? 1.f : 0.f, 20.f);
}

/* ═══════════════════════════════════════════════════════════════════
   LOGO HELPER
═══════════════════════════════════════════════════════════════════ */
static void DrawCoffeeLogo(float cx, float cy, float radius, float time) {
  // Hiệu ứng tỏa sáng nhẹ (Glow)
  for (int i = 0; i < 3; i++) {
    DrawCircle((int)cx, (int)cy, radius * (1.1f + i * 0.1f),
               (Color){120, 170, 255, (unsigned char)(30 - i * 10)});
  }

  // Các vòng tròn đồng tâm xen kẽ Xanh - Trắng
  DrawCircle((int)cx, (int)cy, radius, (Color){90, 140, 255, 255}); // Xanh
  DrawCircle((int)cx, (int)cy, radius * 0.75f, WHITE);              // Trắng
  DrawCircle((int)cx, (int)cy, radius * 0.50f,
             (Color){90, 140, 255, 255});              // Xanh
  DrawCircle((int)cx, (int)cy, radius * 0.25f, WHITE); // Trắng

  // Hiệu ứng khói (Steam) động
  for (int i = 0; i < 3; i++) {
    float ox = (i - 1) * (radius * 0.2f);
    float phase = time * 2.5f + i * 0.8f;
    float sy = cy - radius * 0.4f + sinf(phase) * 3.5f;
    DrawLineEx((Vector2){cx + ox, cy - radius * 0.4f},
               (Vector2){cx + ox + 1.0f, sy - (radius * 0.15f)}, 2.0f,
               (Color){200, 220, 255, 120});
  }
}

/* ═══════════════════════════════════════════════════════════════════
   NÚT BẤM (trả về true khi click)
═══════════════════════════════════════════════════════════════════ */
static bool Button(Rectangle r, const char *label, Color bg, Color bgHov,
                   int slot) {
  Vector2 mp = GetMousePosition();
  bool hov = CheckCollisionPointRec(mp, r);
  bool clk = hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
  updateHov(slot, hov);
  float t = gHov[slot];

  Color dc = {(unsigned char)lerp((float)bg.r, (float)bgHov.r, t),
              (unsigned char)lerp((float)bg.g, (float)bgHov.g, t),
              (unsigned char)lerp((float)bg.b, (float)bgHov.b, t), 255};
  /* bóng đổ */
  DrawRectangleRounded((Rectangle){r.x + 2, r.y + 3, r.width, r.height}, 0.3f,
                       8, (Color){120, 160, 255, (unsigned char)(20 * t)});
  DrawRectangleRounded(r, 0.45f, 8, dc);
  /* viền sáng khi hover */
  if (t > 0.02f) {
    Color bc = {CA_GOLD_LITE.r, CA_GOLD_LITE.g, CA_GOLD_LITE.b,
                (unsigned char)(140 * t)};
    DrawRectangleRoundedLines(r, 0.45f, 8, bc);
  }
  DrawTxtCenter(label, r, 21.f, CT_WHITE, false);
  if (hov)
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  return clk;
}

/* Nút nhỏ (inline bảng) */
static bool SmallBtn(Rectangle r, const char *label, Color bg, Color bgH,
                     int slot) {
  Vector2 mp = GetMousePosition();
  bool hov = CheckCollisionPointRec(mp, r);
  bool clk = hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
  updateHov(slot, hov);
  float t = gHov[slot];
  Color dc = {(unsigned char)lerp((float)bg.r, (float)bgH.r, t),
              (unsigned char)lerp((float)bg.g, (float)bgH.g, t),
              (unsigned char)lerp((float)bg.b, (float)bgH.b, t), 255};
  DrawRectangleRounded(r, 0.35f, 6, dc);
  DrawTxtCenter(label, r, 18.f, CT_WHITE, false);
  if (hov)
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
  return clk;
}

/* ═══════════════════════════════════════════════════════════════════
   Ô NHẬP LIỆU
═══════════════════════════════════════════════════════════════════ */
static void InputField(Rectangle r, int idx, const char *placeholder,
                       float fs) {
  Vector2 mp = GetMousePosition();
  bool hov = CheckCollisionPointRec(mp, r);
  bool active = (gActiveInp == idx);
  if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    gActiveInp = idx;
    active = true;
  }
  Color border = active ? CA_GOLD : (hov ? CB_BORDER2 : CB_BORDER);
  DrawRectangleRounded(r, 0.25f, 8, CB_INPUT);
  DrawRectangleRoundedLines(r, 0.25f, 8, border);

  const char *display = gInp[idx];
  static char masked[1024];
  if (gInpPass[idx]) {
    int nc = 0;
    for (const char *p = gInp[idx]; *p;) {
      if ((*p & 0xC0) != 0x80) {
        masked[nc++] = '*';
      }
      p++;
    }
    masked[nc] = 0;
    display = masked;
  }

  float ty = r.y + (r.height - fs) * 0.5f;
  if (gInpLen[idx] > 0)
    DrawTxtL(display, r.x + 12.f, ty, fs, CT_WHITE);
  else if (!active)
    DrawTxtL(placeholder, r.x + 12.f, ty, fs, CT_DIM);

  /* cursor nhấp nháy */
  if (active && (int)(gCursorBlink * 2) % 2 == 0) {
    Vector2 m = Measure(display, fs);
    DrawRectangle((int)(r.x + 12 + m.x + 2), (int)ty, 2, (int)fs, CA_GOLD);
  }
  if (hov)
    SetMouseCursor(MOUSE_CURSOR_IBEAM);
}

/* ═══════════════════════════════════════════════════════════════════
   XỬ LÝ PHÍM / INPUT
═══════════════════════════════════════════════════════════════════ */
/* ═══════════════════════════════════════════════════════════════════
   XỬ LÝ PHÍM / INPUT
═══════════════════════════════════════════════════════════════════ */
static void processInput(void) {
  gCursorBlink += GetFrameTime();
  if (gCursorBlink > 2.f)
    gCursorBlink = 0;
  int idx = gActiveInp;
  if (idx < 0 || idx >= MAX_INP)
    return;
  if (IsKeyPressed(KEY_ESCAPE)) {
    gActiveInp = -1;
    return;
  }
  if (IsKeyPressed(KEY_TAB)) {
    gActiveInp = (gActiveInp + 1) % MAX_INP;
    return;
  }

  /* TỰ ĐỘNG CHUYỂN SANG Ô PASSWORD KHI NHẤN ENTER Ở Ô USERNAME (MÀN HÌNH LOGIN)
   */
  if (gScreen == SCR_LOGIN && idx == 0 &&
      (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) {
    gActiveInp = 1; // Di chuyển tiêu điểm con trỏ xuống ô Password

    /* BÍ QUYẾT: Nuốt phím bằng cách ép bộ đệm Raylib đọc trạng thái hiện tại
       ngay lập tức, ngăn không cho hàm drawLogin() phía sau nhận diện nhầm phím
       Enter ở Frame này */
    while (GetCharPressed() > 0)
      ;
    return;
  }

  if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
    utf8DeleteLast(gInp[idx], &gInpLen[idx]);
  int cp = GetCharPressed();
  while (cp > 0) {
    char buf[5] = {0};
    int nb = utf8Encode(cp, buf);
    if (nb > 0 && gInpLen[idx] + nb < 1020) {
      for (int i = 0; i < nb; i++)
        gInp[idx][gInpLen[idx]++] = buf[i];
      gInp[idx][gInpLen[idx]] = 0;
    }
    cp = GetCharPressed();
  }
}
/* ═══════════════════════════════════════════════════════════════════
   CẬP NHẬT CUỘN
═══════════════════════════════════════════════════════════════════ */
static void updateScroll(Rectangle area, float contentH) {
  float visH = area.height;
  float maxS = fmaxf(0.f, contentH - visH);
  if (CheckCollisionPointRec(GetMousePosition(), area)) {
    gScrollTarget -= GetMouseWheelMove() * 72.f;
  }
  gScrollTarget = fmaxf(0.f, fminf(gScrollTarget, maxS));
  gScrollY = smoothStep(gScrollY, gScrollTarget, 22.f);
}

/* ═══════════════════════════════════════════════════════════════════
   SIDEBAR
═══════════════════════════════════════════════════════════════════ */
static void drawSidebar(void) {
  int sw = SIDEBAR_W;
  DrawRectangle(0, 0, sw, WH, CB_SIDEBAR);
  DrawRectangle(sw - 1, 0, 1, WH, CB_BORDER);

  /* ── Logo / Header ── */
  DrawRectangle(0, 0, sw, 145, CB_BG);

  /* Coffee icon animated */
  DrawCoffeeLogo(sw * 0.5f, 50.f, 30.f, gElapsed);
  /* title */
  Vector2 tv = MeasureB("ITF COFFEE", 22.f);
  DrawTextEx(gFontB, "ITF COFFEE", (Vector2){(sw - tv.x) * .5f, 82.f}, 22.f,
             1.f, CA_GOLD);
  Vector2 sv = Measure("Quản Lý Quán", 14);
  DrawTxtL("Quản Lý Quán", (sw - sv.x) * .5f, 110.f, 14.f, CT_DIM);

  /* ── Thẻ nhân viên đang trực ── */
  int cardY = 148;
  if (gCurrentStaff) {
    DrawRectangleRounded((Rectangle){8, cardY, sw - 16, 70}, 0.12f, 6,
                         CB_PANEL);
    DrawRectangleRoundedLines((Rectangle){8, cardY, sw - 16, 70}, 0.12f, 6,
                              CB_BORDER);
    /* tên */
    Vector2 nv = MeasureB(gCurrentStaff->name, 15.f);
    DrawTextEx(gFontB, gCurrentStaff->name,
               (Vector2){(sw - nv.x) * 0.5f, cardY + 10.f}, 15.f, 1.f,
               CT_WHITE);
    /* chức vụ */
    Vector2 pv = Measure(gCurrentStaff->position, 13.f);
    DrawTxtL(gCurrentStaff->position, (sw - pv.x) * .5f, cardY + 42.f, 13.f,
             CA_GOLD);
  }

  /* ── Menu điều hướng ── */
  typedef struct {
    const char *label;
    Screen scr;
    bool logout;
  } NavItem;
  NavItem nav[] = {
      {"  Sơ đồ Bàn", SCR_TABLES, false}, {"  Thực đơn", SCR_MENU, false},
      {"  Nhân viên", SCR_STAFF, false},  {"  Thống kê", SCR_STATS, false},
      {"  Biểu đồ", SCR_CHART, false},    {"", -1, false}, /* separator */
      {"  Đăng xuất", -1, true},
  };
  int startY = gCurrentStaff ? 226 : 152;
  int bH = 50, bPad = 8;

  for (int i = 0; i < 7; i++) {
    if (!nav[i].label[0])
      continue; /* separator */
    Rectangle r = {(float)bPad, (float)(startY + i * (bH + 4)),
                   (float)(sw - bPad * 2), (float)bH};
    Vector2 mp = GetMousePosition();
    bool hov = CheckCollisionPointRec(mp, r);
    bool isAct = !nav[i].logout && gScreen == nav[i].scr;

    Color bg = isAct ? (Color){120, 170, 255, 255}
                     : (hov ? (Color){210, 225, 255, 255} : CB_SIDEBAR);
    DrawRectangleRounded(r, 0.35f, 6, bg);

    /* indicateur actif */
    if (isAct)
      DrawRectangle(bPad, startY + i * (bH + 4) + 6, 3, bH - 12, CA_GOLD);

    Color tc = nav[i].logout ? (hov ? CS_ERR : (Color){255, 120, 170, 255})
                             : (isAct ? CA_GOLD : (hov ? CT_WHITE : CT_MUTED));
    DrawTxtVCL(nav[i].label, r, 21.f, tc, 21.f, false);

    if (hov) {
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (nav[i].logout) {
          gCurrentStaff = NULL;
          resetScreen(SCR_LOGIN);
        } else
          resetScreen(nav[i].scr);
      }
    }
  }

  /* version */
  Vector2 vv = Measure("v2.0  Raylib", 13);
  DrawTxtL("v2.0  Raylib", (sw - vv.x) * .5f, WH - 20.f, 13.f, CT_DIM);
}

/* ═══════════════════════════════════════════════════════════════════
   TOP BAR
═══════════════════════════════════════════════════════════════════ */
static void drawTopBar(const char *title) {
  int ax = SIDEBAR_W;
  DrawRectangle(ax, 0, WW - ax, TOPBAR_H, CB_TOPBAR);
  DrawRectangle(ax, TOPBAR_H - 1, WW - ax, 1, CB_BORDER);

  Vector2 tv = MeasureB(title, 22);
  DrawTextEx(gFontB, title, (Vector2){ax + 24.f, (TOPBAR_H - tv.y) * .5f}, 22.f,
             1.f, CA_GOLD);

  time_t now = time(NULL);
  struct tm *ti = localtime(&now);
  char ts[32];
  strftime(ts, 32, "%H:%M  %d/%m/%Y", ti);
  Vector2 tv2 = Measure(ts, 19);
  DrawTxtL(ts, WW - tv2.x - 24.f, (TOPBAR_H - tv2.y) * 0.5f, 19.f, CT_MUTED);
}

/* ═══════════════════════════════════════════════════════════════════
   MÀN HÌNH ĐĂNG NHẬP
═══════════════════════════════════════════════════════════════════ */
static void drawLogin(void) {
  /* Background glow circles */
  for (int i = 0; i < 12; i++) {
    float px = WW * .5f + sinf(gElapsed * .22f + i * 1.05f) * 480;
    float py = WH * .5f + cosf(gElapsed * .16f + i * .95f) * 280;
    DrawCircle((int)px, (int)py, 32, (Color){120, 170, 255, 35});
  }

  int cw = 580, ch = 520;
  int cx = (WW - cw) / 2, cy = (WH - ch) / 2;
  Rectangle card = {(float)cx, (float)cy, (float)cw, (float)ch};

  DrawRectangleRounded((Rectangle){card.x + 8, card.y + 10, cw, ch}, 0.16f, 12,
                       (Color){120, 160, 255, 25});
  DrawRectangleRounded(card, 0.16f, 12, CB_PANEL);
  DrawRectangleRoundedLines(card, 0.16f, 12, CB_BORDER);

  /* Header card */
  DrawRectangleRounded((Rectangle){card.x, card.y, (float)cw, 120}, 0.16f, 12,
                       (Color){225, 235, 255, 255});

  /* Coffee icon */
  DrawCoffeeLogo(cx + cw * 0.5f, cy + 44.f, 26.f, gElapsed);

  Vector2 gtv = MeasureB("ITF COFFEE", 39);
  DrawTextEx(gFontB, "ITF COFFEE",
             (Vector2){cx + (cw - gtv.x) * .5f, cy + 76.f}, 39.f, 1.f, CA_GOLD);

  const char *sub = "HỆ THỐNG QUẢN LÝ QUÁN CÀ PHÊ";
  Vector2 stv = Measure(sub, 16);
  DrawTxtL(sub, cx + (cw - stv.x) * .5f, cy + 124.f, 16.f, CT_MUTED);

  /* Form inputs */
  int fx = cx + 48, fw = cw - 96;
  DrawTxtL("Tên đăng nhập", (float)fx, cy + 150.f, 16.f, CT_MUTED);
  InputField((Rectangle){(float)fx, (float)(cy + 172), fw, 52}, 0,
             "Nhập username...", 21);

  DrawTxtL("Mật khẩu", (float)fx, cy + 240.f, 16.f, CT_MUTED);
  gInpPass[1] = true;
  InputField((Rectangle){(float)fx, (float)(cy + 262), fw, 52}, 1,
             "Nhập password...", 21);

  /* Chi giu lai su kien click chuot vao nut button dang nhap */
  bool clk = Button((Rectangle){(float)fx, (float)(cy + 342), (float)fw, 58},
                    "ĐĂNG NHẬP", CA_GOLD_DIM, CA_GOLD, 0);

  if (clk) {
    bool found = false;
    for (int i = 0; i < gStaffCount; i++) {
      if (strcmp(gStaff[i].username, gInp[0]) == 0 &&
          strcmp(gStaff[i].password, gInp[1]) == 0) {
        gCurrentStaff = &gStaff[i];
        resetScreen(SCR_TABLES);
        char msg[80];
        sprintf(msg, "Xin chào, %s!", gCurrentStaff->name);
        showToast(msg, CS_OK);
        found = true;
        break;
      }
    }
    if (!found)
      showToast("Sai tên đăng nhập hoặc mật khẩu!", CS_ERR);
  }

  const char *hint = "Tài khoản mặc định:  admin / 123";
  DrawTxtL(hint, (float)(cx + 24), cy + ch - 28.f, 14.f, CT_DIM);
}
/* ═══════════════════════════════════════════════════════════════════
   SƠ ĐỒ BÀN
═══════════════════════════════════════════════════════════════════ */
static void drawTableMap(void) {
  drawSidebar();
  drawTopBar("SƠ ĐỒ BÀN");
  int ax = SIDEBAR_W, ay = TOPBAR_H;
  int aw = WW - ax, ah = WH - ay;
  DrawRectangle(ax, ay, aw, ah, CB_BG);

  /* KPI row */
  int occ = 0;
  float liveBill = 0;
  for (int i = 0; i < MAX_TABLES; i++)
    if (gTables[i].isOccupied) {
      occ++;
      liveBill += gTables[i].currentBill;
    }

  struct {
    const char *lbl, *val;
    Color col;
  } kpis[3];
  char k1[16], k2[16], k3[32];
  sprintf(k1, "%d / %d", occ, MAX_TABLES);
  kpis[0] = (typeof(kpis[0])){"Đang phục vụ", k1, CS_ERR};
  sprintf(k2, "%d / %d", MAX_TABLES - occ, MAX_TABLES);
  kpis[1] = (typeof(kpis[1])){"Bàn trống", k2, CS_OK};
  sprintf(k3, "%.0f VND", liveBill);
  kpis[2] = (typeof(kpis[2])){"Tạm thu", k3, CA_GOLD};

  int kw = (aw - 60) / 3, kh = 90;
  for (int i = 0; i < 3; i++) {
    Rectangle kr = {(float)(ax + 16 + i * (kw + 14)), (float)(ay + 12),
                    (float)kw, (float)kh};
    DrawRectangleRounded(kr, 0.12f, 8, CB_PANEL);
    DrawRectangle((int)kr.x, (int)kr.y, 3, kh, kpis[i].col);
    DrawTxtL(kpis[i].lbl, kr.x + 16, kr.y + 14, 27.f, CT_MUTED);
    DrawTextEx(gFontB, kpis[i].val, (Vector2){kr.x + 16, kr.y + 40}, 30.f, 1.f,
               kpis[i].col);
  }

  /* Lưới bàn 5×2 */
  int cols = 5, rows = 2, pad = 16;
  int tw = (aw - pad * (cols + 1)) / cols, th2 = 178;
  int gx = ax + pad, gy = ay + 116;

  for (int i = 0; i < MAX_TABLES; i++) {
    int col = i % cols, row = i / cols;
    float fx = (float)(gx + col * (tw + pad));
    float fy = (float)(gy + row * (th2 + pad));
    Rectangle r = {(float)fx, (float)fy, (float)tw, (float)th2};
    Vector2 mp = GetMousePosition();
    bool hov = CheckCollisionPointRec(mp, r);
    updateHov(10 + i, hov);
    float t = gHov[10 + i];
    bool occ2 = gTables[i].isOccupied;

    /* shadow */
    DrawRectangleRounded(
        (Rectangle){r.x + 3, r.y + 4, (float)tw, (float)th2}, 0.12f, 8,
        (Color){120, 160, 255, (unsigned char)(28 * (0.3f + t * .7f))});

    Color bg = {(unsigned char)lerp((float)CB_CARD.r, (float)CB_CARD_HOV.r, t),
                (unsigned char)lerp((float)CB_CARD.g, (float)CB_CARD_HOV.g, t),
                (unsigned char)lerp((float)CB_CARD.b, (float)CB_CARD_HOV.b, t),
                255};
    DrawRectangleRounded(r, 0.12f, 8, bg);

    /* top stripe */
    Color sc = occ2 ? CS_ERR : CS_OK;
    DrawRectangleRounded((Rectangle){r.x + 1, r.y, (float)(tw - 2), 7}, 0.9f, 4,
                         sc);

    /* số bàn */
    char tn[16];
    sprintf(tn, "BÀN %02d", gTables[i].id);
    Vector2 tnv = MeasureB(tn, 17);
    DrawTextEx(gFontB, tn, (Vector2){fx + (tw - tnv.x) * .5f, fy + 14}, 17.f,
               1.f, CT_WHITE);

    /* trạng thái */
    const char *stxt = occ2 ? "[+] Đang phục vụ" : "[ ] Trống";
    Vector2 sv2 = Measure(stxt, 16);
    DrawTxtL(stxt, fx + (tw - sv2.x) * .5f, fy + 42, 16.f, sc);

    if (occ2) {
      char bs[24];
      sprintf(bs, "%.0f d", gTables[i].currentBill);
      Vector2 bv = MeasureB(bs, 18);
      DrawTextEx(gFontB, bs, (Vector2){fx + (tw - bv.x) * .5f, fy + 68}, 18.f,
                 1.f, CA_GOLD);
      char sn[STR_LEN + 6];
      snprintf(sn, STR_LEN + 5, "NV: %s", gTables[i].staffName);
      Vector2 snv = Measure(sn, 13);
      DrawTxtL(sn, fx + (tw - snv.x) * .5f, fy + 100, 13.f, CT_MUTED);
      char ni[16];
      sprintf(ni, "%d mon", gTables[i].itemCount);
      Vector2 niv = Measure(ni, 13);
      DrawTxtL(ni, fx + (tw - niv.x) * .5f, fy + 118, 13.f, CT_DIM);
    }

    /* hover border glow */
    if (t > 0.02f) {
      Color bc = {CA_GOLD.r, CA_GOLD.g, CA_GOLD.b, (unsigned char)(160 * t)};
      DrawRectangleRoundedLines(r, 0.12f, 8, bc);
    }

    if (hov) {
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        gSelTable = i;
        resetScreen(SCR_ORDER);
      }
    }
  }

  /* Legend */
  float ly = gy + rows * (th2 + pad) + 4;
  DrawCircle(ax + 20, (int)(ly + 9), 6, CS_OK);
  DrawTxtL("Bàn trống", ax + 32, ly, 16.f, CT_MUTED);
  DrawCircle(ax + 140, (int)(ly + 9), 6, CS_ERR);
  DrawTxtL("Đang phục vụ", ax + 152, ly, 16.f, CT_MUTED);
  DrawTxtL(">> Click vao ban de goi mon", ax + 310, ly, 16.f, CT_DIM);
}

/* ═══════════════════════════════════════════════════════════════════
   MÀN HÌNH GỌI MÓN
═══════════════════════════════════════════════════════════════════ */
static void drawOrder(void) {
  drawSidebar();
  int t = gSelTable;
  char ttl[48];
  sprintf(ttl, "GỌI MÓN - BÀN %02d", gTables[t].id);
  drawTopBar(ttl);

  int ax = SIDEBAR_W, ay = TOPBAR_H;
  int aw = WW - ax, ah = WH - ay;
  int menuW = (int)(aw * 0.60f), ordW = aw - menuW;
  int menuX = ax, ordX = ax + menuW;

  DrawRectangle(menuX, ay, menuW, ah, CB_BG);
  DrawRectangle(menuX + menuW - 1, ay, 1, ah, CB_BORDER);

  /* Tìm kiếm */
  DrawTxtL("Tìm món:", menuX + 14, ay + 10, 15.f, CT_MUTED);
  InputField((Rectangle){menuX + 14, ay + 30, menuW - 28, 38}, 0,
             "Nhập tên món...", 13);

  /* Danh sach thuc don (co cuon) */
  int itemH = 90, listStartY = ay + 80, visH = ah - 80;
  Rectangle listArea = {(float)menuX, (float)listStartY, (float)menuW,
                        (float)visH};
  updateScroll(listArea, (float)(gMenuCount * itemH));

  BeginScissorMode(menuX, listStartY, menuW, visH);
  int vi = 0;
  for (int i = 0; i < gMenuCount; i++) {
    if (gInpLen[0] > 0 && !matchSearch(gInp[0], gMenu[i].name))
      continue;
    float fy = (float)(listStartY + vi * itemH) - gScrollY;
    vi++;
    if (fy + itemH < listStartY || fy > listStartY + visH)
      continue;

    Rectangle r = {(float)(menuX + 8), fy, (float)(menuW - 16),
                   (float)(itemH - 4)};
    Vector2 mp = GetMousePosition();
    bool hov = CheckCollisionPointRec(mp, r);
    bool oos = !gMenu[i].inStock;
    Color bg = oos ? (Color){26, 14, 6, 200} : (hov ? CB_CARD_HOV : CB_CARD);
    DrawRectangleRounded(r, 0.15f, 6, bg);

    char ids[8];
    sprintf(ids, "#%d", gMenu[i].id);
    DrawTxtL(ids, r.x + 10, r.y + 5, 14.f, CT_DIM);
    DrawTxtL(gMenu[i].name, r.x + 10, r.y + 24, 18.f, oos ? CT_DIM : CT_WHITE);
    char ps[24];
    sprintf(ps, "%.0f đ", gMenu[i].price);
    DrawTxtL(ps, r.x + 10, r.y + 48, 16.f, CA_GOLD);

    if (oos) {
      Vector2 hv = Measure("Hết nguyên liệu", 15);
      DrawTxtL("Hết nguyên liệu", r.x + r.width - hv.x - 10,
               r.y + (itemH - 4 - 15) * .5f, 15.f, CS_ERR);
    } else {
      Rectangle ab = {(float)(menuX + menuW - 84), fy + 8, 72,
                      (float)(itemH - 20)};
      int slot = 30 + i; /* button slot */
      if (Button(ab, "+ Thêm", CA_GOLD_DIM, CA_GOLD, slot)) {
        if (!gTables[t].isOccupied) {
          gTables[t].isOccupied = 1;
          if (gCurrentStaff)
            strcpy(gTables[t].staffName, gCurrentStaff->name);
        }
        bool found2 = false;
        for (int j = 0; j < gTables[t].itemCount; j++) {
          if (gTables[t].items[j].menuId == gMenu[i].id) {
            gTables[t].items[j].qty++;
            gTables[t].currentBill += gMenu[i].price;
            found2 = true;
            break;
          }
        }
        if (!found2 && gTables[t].itemCount < MAX_ITEMS) {
          int n = gTables[t].itemCount;
          gTables[t].items[n].menuId = gMenu[i].id;
          strncpy(gTables[t].items[n].name, gMenu[i].name, STR_LEN - 1);
          gTables[t].items[n].price = gMenu[i].price;
          gTables[t].items[n].qty = 1;
          gTables[t].itemCount++;
          gTables[t].currentBill += gMenu[i].price;
        }
        char msg[64];
        sprintf(msg, "Đã thêm: %s", gMenu[i].name);
        showToast(msg, CS_OK);
      }
    }
  }
  EndScissorMode();

    /* ── Tổng tiền — panel nổi bật ── */
    int panelH   = 164;                          /* chiều cao khu vực phía dưới */
    int panelY   = ay + ah - panelH;

    /* đường phân cách nhẹ */
    DrawRectangle(ordX, panelY, ordW, 1, CB_BORDER2);

    /* nền tổng cộng */
    DrawRectangleRounded((Rectangle){ordX+8, panelY+8, ordW-16, 54}, 0.18f, 8,
                         (Color){225,235,255,255});

    /* nhãn bên trái */
    DrawTxtVCL("TỔNG CỘNG",
               (Rectangle){ordX+8, panelY+8, ordW-16, 54},
               16.f, CT_MUTED, 14.f, false);

    /* số tiền bên phải — to, nổi */
    char tot[32]; sprintf(tot,"%.0f VND", gTables[t].currentBill);
    Vector2 totv = MeasureB(tot, 24);
    float totX   = ordX + ordW - totv.x - 14;
    float totY   = panelY + 8 + (54 - totv.y) * 0.5f;
    DrawTextEx(gFontB, tot, (Vector2){totX, totY}, 24.f, 1.f, CA_GOLD);

    /* ── Nút hành động ── */
    float bW = (ordW - 20) * 0.5f;
    if (Button((Rectangle){ordX+6,  panelY+72, bW,   44},"Quay lại",   CB_CARD,     CB_CARD_HOV,  4))
        resetScreen(SCR_TABLES);
    if (Button((Rectangle){ordX+14+bW, panelY+72, bW, 44},"Thanh toán", CA_GOLD_DIM, CA_GOLD,      5)){
        if (gTables[t].itemCount>0) { gScreen=SCR_INVOICE; }
        else showToast("Bàn chưa có món nào!",CS_WARN);
    }
    if (Button((Rectangle){ordX+6, panelY+122, ordW-12, 36},"Làm mới đơn",
               CB_CARD,(Color){45,26,12,255},6)){
        if (gTables[t].itemCount>0)
            openDialog("Xác nhận làm mới","Xóa hết đơn hiện tại của bàn này?",300+t);
    }
  }
  EndScissorMode();

  /* Tổng tiền */
  int totLineY = ay + ah - 110;
  DrawRectangle(ordX, totLineY - 8, ordW, 1, CB_BORDER);
  char tot[32];
  sprintf(tot, "%.0f VND", gTables[t].currentBill);
  DrawTxtL("TỔNG CỘNG:", ordX + 10, totLineY, 17.f, CT_MUTED);
  Vector2 totv = MeasureB(tot, 22);
  DrawTextEx(gFontB, tot, (Vector2){ordX + ordW - totv.x - 10, totLineY - 3},
             22.f, 1.f, CA_GOLD);

  /* Nút hành động */
  float bW = (ordW - 20) * .5f;
  if (Button((Rectangle){ordX + 6, ay + ah - 94, bW, 40}, "Quay lại", CB_CARD,
             CB_CARD_HOV, 4))
    resetScreen(SCR_TABLES);
  if (Button((Rectangle){ordX + 14 + bW, ay + ah - 94, bW, 40}, "Thanh toán",
             CA_GOLD_DIM, CA_GOLD, 5)) {
    if (gTables[t].itemCount > 0) {
      gScreen = SCR_INVOICE;
    } else
      showToast("Bàn chưa có món nào!", CS_WARN);
  }
  if (Button((Rectangle){ordX + 6, ay + ah - 48, ordW - 12, 38}, "Làm mới đơn",
             CB_CARD, (Color){45, 26, 12, 255}, 6)) {
    if (gTables[t].itemCount > 0)
      openDialog("Xác nhận làm mới", "Xóa hết đơn hiện tại của bàn này?",
                 300 + t);
  }
}

/* ═══════════════════════════════════════════════════════════════════
   HÓA ĐƠN
═══════════════════════════════════════════════════════════════════ */
static void drawInvoice(void) {
  drawSidebar();
  int t = gSelTable;
  char ttl[48];
  sprintf(ttl, "HÓA ĐƠN - BÀN %02d", gTables[t].id);
  drawTopBar(ttl);

  int ax = SIDEBAR_W, ay = TOPBAR_H, aw = WW - ax;
  DrawRectangle(ax, ay, aw, WH - ay, CB_BG);

  int iw = 860;
  int ih = fmaxf(500, 320 + gTables[t].itemCount * 40 + 80);
  int ix = ax + (aw - iw) / 2, iy = ay + 20;

  DrawRectangleRounded((Rectangle){ix + 6, iy + 8, iw, ih}, 0.05f, 12,
                       (Color){0, 0, 0, 100});
  DrawRectangleRounded((Rectangle){ix, iy, iw, ih}, 0.05f, 12, CB_PANEL);
  DrawRectangleRoundedLines((Rectangle){ix, iy, iw, ih}, 0.05f, 12, CB_BORDER);

  /* Header */
  DrawRectangleRounded((Rectangle){ix, iy, iw, 96}, 0.05f, 12,
                       (Color){225, 235, 255, 255});
  Vector2 htv = MeasureB("HÓA ĐƠN THANH TOÁN", 22);
  DrawTextEx(gFontB, "HÓA ĐƠN THANH TOÁN",
             (Vector2){ix + (iw - htv.x) * .5f, iy + 16}, 22.f, 1.f, CA_GOLD);
  const char *sub2 = "ITF COFFEE  -  Hệ thống Quản lý Quán Cà Phê";
  Vector2 sv3 = Measure(sub2, 14);
  DrawTxtL(sub2, ix + (iw - sv3.x) * .5f, iy + 52, 14.f, CT_MUTED);

  int py = iy + 108;
  time_t now = time(NULL);
  struct tm *ti = localtime(&now);
  char ts3[32];
  strftime(ts3, 32, "%H:%M:%S  %d/%m/%Y", ti);
  char info[80];
  sprintf(info, "Thời gian: %s", ts3);
  DrawTxtL(info, ix + 32, py, 15.f, CT_WHITE);
  py += 28;
  sprintf(info, "Bàn số: %d", gTables[t].id);
  DrawTxtL(info, ix + 32, py, 15.f, CT_WHITE);
  py += 28;
  sprintf(info, "Thu ngân: %s", gCurrentStaff ? gCurrentStaff->name : "-");
  DrawTxtL(info, ix + 32, py, 15.f, CT_WHITE);
  py += 24;
  DrawLine(ix + 32, py + 4, ix + iw - 32, py + 4, CB_BORDER);
  py += 18;

  DrawTxtBL("Tên món", ix + 32, py, 14.f, CT_MUTED);
  DrawTxtBL("Đơn giá", ix + 400, py, 14.f, CT_MUTED);
  DrawTxtBL("SL", ix + 580, py, 14.f, CT_MUTED);
  DrawTxtBL("Thành tiền", ix + 650, py, 14.f, CT_MUTED);
  py += 22;
  DrawLine(ix + 32, py, ix + iw - 32, py, CB_BORDER);
  py += 10;

  for (int i = 0; i < gTables[t].itemCount; i++) {
    DrawTxtL(gTables[t].items[i].name, ix + 32, py, 15.f, CT_WHITE);
    char pp[20];
    sprintf(pp, "%.0f", gTables[t].items[i].price);
    DrawTxtL(pp, ix + 400, py, 15.f, CT_MUTED);
    char qq[8];
    sprintf(qq, "%d", gTables[t].items[i].qty);
    DrawTxtL(qq, ix + 588, py, 15.f, CT_MUTED);
    char tt2[24];
    sprintf(tt2, "%.0f VND",
            gTables[t].items[i].price * gTables[t].items[i].qty);
    Vector2 tv3 = Measure(tt2, 15);
    DrawTxtL(tt2, ix + iw - 32 - tv3.x, py, 15.f, CT_WHITE);
    py += 34;
  }

  DrawLine(ix + 32, py + 2, ix + iw - 32, py + 2, CB_BORDER);
  py += 18;
  DrawTxtBL("TỔNG CỘNG:", ix + 32, py, 18.f, CT_WHITE);
  char tot2[32];
  sprintf(tot2, "%.0f VND", gTables[t].currentBill);
  Vector2 tv4 = MeasureB(tot2, 28);
  DrawTextEx(gFontB, tot2, (Vector2){ix + iw - 32 - tv4.x, py - 2}, 28.f, 1.f,
             CA_GOLD);
  py += 56;

  if (Button((Rectangle){ix + 32, py, iw / 2 - 42, 52}, "Quay lại chỉnh sửa",
             CB_CARD, CB_CARD_HOV, 7)) {
    gScreen = SCR_ORDER;
  }
  if (Button((Rectangle){ix + iw / 2 + 10, py, iw / 2 - 42, 52},
             "[OK]  Xác nhận Thanh toán", CA_GOLD_DIM, CA_GOLD, 8)) {
    gTotalRevenue += gTables[t].currentBill;
    saveInvoice(t);
    gTables[t].isOccupied = 0;
    gTables[t].currentBill = 0;
    gTables[t].itemCount = 0;
    memset(gTables[t].items, 0, sizeof(gTables[t].items));
    showToast("Thanh toán thành công! Hóa đơn đã được lưu.", CS_OK);
    resetScreen(SCR_TABLES);
  }
}

/* ═══════════════════════════════════════════════════════════════════
   QUẢN LÝ THỰC ĐƠN
═══════════════════════════════════════════════════════════════════ */
static void drawMenuManage(void) {
  drawSidebar();
  drawTopBar("QUẢN LÝ THỰC ĐƠN");
  int ax = SIDEBAR_W, ay = TOPBAR_H, aw = WW - ax, ah = WH - ay;
  DrawRectangle(ax, ay, aw, ah, CB_BG);

  if (gSubScr == 0) {
    /* Toolbar */
    InputField((Rectangle){ax + 14, ay + 12, 280, 36}, 0, "Tìm kiếm món ăn...",
               13);
    if (Button((Rectangle){WW - 132, ay + 12, 116, 36}, "+ Thêm món",
               CA_GOLD_DIM, CA_GOLD, 0)) {
      gSubScr = 1;
      memset(gInp, 0, sizeof(gInp));
      memset(gInpLen, 0, sizeof(gInpLen));
      memset(gInpPass, 0, sizeof(gInpPass));
    }

    /* Table header */
    int hY = ay + 56;
    DrawRectangle(ax, hY, aw, 40, CB_PANEL);
    struct {
      const char *lbl;
      int x;
    } cols[] = {{"ID", ax + 12},
                {"Tên món", ax + 64},
                {"Giá (VND)", ax + 362},
                {"Trạng thái", ax + 510},
                {"", ax + 680}};
    for (int i = 0; i < 5; i++)
      DrawTextEx(gFontB, cols[i].lbl, (Vector2){cols[i].x, hY + 12}, 16.f, 1.f,
                 CT_MUTED);
    DrawRectangle(ax, hY + 40, aw, 1, CB_BORDER);

    /* Rows */
    int rowH = 84, startY = hY + 42, visH = ah - 56 - 42;
    Rectangle la = {(float)ax, (float)startY, (float)aw, (float)visH};
    updateScroll(la, (float)(gMenuCount * rowH));

    BeginScissorMode(ax, startY, aw, visH);
    int vi = 0;
    for (int i = 0; i < gMenuCount; i++) {
      if (gInpLen[0] > 0 && !matchSearch(gInp[0], gMenu[i].name))
        continue;
      float fy = (float)(startY + vi * rowH) - gScrollY;
      vi++;
      if (fy + rowH < startY || fy > startY + visH)
        continue;
      Color rbg = (vi % 2 == 0) ? CB_ROW_ALT : CB_BG;
      DrawRectangle(ax, (int)fy, aw, rowH, rbg);
      DrawLine(ax, (int)(fy + rowH - 1), ax + aw, (int)(fy + rowH - 1),
               CB_BORDER);
      float ty = fy + (rowH - 17) * .5f;
      char ids[8];
      sprintf(ids, "%d", gMenu[i].id);
      DrawTxtL(ids, ax + 12, ty, 17.f, CT_DIM);
      DrawTxtL(gMenu[i].name, ax + 64, ty, 17.f, CT_WHITE);
      char ps[20];
      sprintf(ps, "%.0f", gMenu[i].price);
      DrawTxtL(ps, ax + 362, ty, 17.f, CA_GOLD);
      const char *stxt = gMenu[i].inStock ? "[+] Còn" : "[-] Hết";
      DrawTxtL(stxt, ax + 510, ty, 16.f, gMenu[i].inStock ? CS_OK : CS_ERR);
      int by = (int)(fy + (rowH - 26) * .5f);
      if (SmallBtn((Rectangle){ax + 664, by, 52, 26}, "Sửa", CB_CARD,
                   CB_CARD_HOV, 20 + i * 3)) {
        gSubScr = 2;
        gEditId = gMenu[i].id;
        strncpy(gInp[0], gMenu[i].name, 1019);
        gInpLen[0] = (int)strlen(gInp[0]);
        char pc[20];
        sprintf(pc, "%.0f", gMenu[i].price);
        strncpy(gInp[1], pc, 1019);
        gInpLen[1] = (int)strlen(gInp[1]);
      }
      const char *tl = gMenu[i].inStock ? "Hết" : "Còn";
      if (SmallBtn((Rectangle){ax + 720, by, 48, 26}, tl, CB_CARD, CB_CARD_HOV,
                   21 + i * 3)) {
        gMenu[i].inStock = !gMenu[i].inStock;
        saveMenu();
        showToast("Đã cập nhật trạng thái!", CS_INFO);
      }
      if (SmallBtn((Rectangle){ax + 772, by, 48, 26}, "Xóa",
                   (Color){255, 120, 160, 255}, CS_ERR, 22 + i * 3)) {
        openDialog("Xác nhận xóa", "Bạn có chắc muốn xóa món này?", 100 + i);
      }
    }
    EndScissorMode();
  } else {
    /* Form thêm/sửa */
    int fw = 560, fh = 380;
    int fx = (WW - SIDEBAR_W - fw) / 2 + SIDEBAR_W, fy = ay + 80;
    DrawRectangleRounded((Rectangle){fx, fy, fw, fh}, 0.16f, 12, CB_PANEL);
    DrawRectangleRoundedLines((Rectangle){fx, fy, fw, fh}, 0.16f, 12,
                              CB_BORDER);
    const char *ft = gSubScr == 1 ? "THÊM MÓN MỚI" : "SỬA THÔNG TIN MÓN";
    Vector2 ftv = MeasureB(ft, 16);
    DrawTextEx(gFontB, ft, (Vector2){fx + (fw - ftv.x) * .5f, fy + 14}, 16.f,
               1.f, CA_GOLD);
    DrawLine(fx + 20, fy + 46, fx + fw - 20, fy + 46, CB_BORDER);

    int px = fx + 28, pw = fw - 56;
    DrawTxtL("Tên món:", px, fy + 54, 16.f, CT_MUTED);
    InputField((Rectangle){px, fy + 72, pw, 44}, 0, "Nhập tên món...", 14);
    DrawTxtL("Giá bán (VND):", px, fy + 130, 16.f, CT_MUTED);
    InputField((Rectangle){px, fy + 148, pw, 44}, 1, "Nhập giá...", 14);

    bool sv = Button((Rectangle){px, fy + 252, (float)(pw / 2 - 8), 52},
                     "[S]  Lưu", CA_GOLD_DIM, CA_GOLD, 1);
    bool cl =
        Button((Rectangle){px + pw / 2 + 8, fy + 252, (float)(pw / 2 - 8), 52},
               "[X]  Hủy", CB_CARD, CB_CARD_HOV, 2);
    if (cl) {
      gSubScr = 0;
      memset(gInp, 0, sizeof(gInp));
      memset(gInpLen, 0, sizeof(gInpLen));
    }
    if (sv) {
      if (!gInpLen[0] || !gInpLen[1])
        showToast("Vui lòng điền đầy đủ thông tin!", CS_WARN);
      else {
        float price = atof(gInp[1]);
        if (gSubScr == 1) {
          if (gMenuCount >= MAX_MENU)
            showToast("Danh sách đã đầy!", CS_ERR);
          else {
            int nid = gMenuCount ? gMenu[gMenuCount - 1].id + 1 : 1;
            gMenu[gMenuCount] = (MenuItem){nid, "", price, 1};
            strncpy(gMenu[gMenuCount].name, gInp[0], STR_LEN - 1);
            gMenuCount++;
            saveMenu();
            showToast("Đã thêm món mới!", CS_OK);
            gSubScr = 0;
          }
        } else {
          for (int j = 0; j < gMenuCount; j++)
            if (gMenu[j].id == gEditId) {
              strncpy(gMenu[j].name, gInp[0], STR_LEN - 1);
              gMenu[j].price = price;
              saveMenu();
              showToast("Đã cập nhật món!", CS_OK);
              gSubScr = 0;
              break;
            }
        }
      }
    }
  }
}

/* ═══════════════════════════════════════════════════════════════════
   QUẢN LÝ NHÂN VIÊN
═══════════════════════════════════════════════════════════════════ */
static void drawStaffManage(void) {
  drawSidebar();
  drawTopBar("QUẢN LÝ NHÂN VIÊN");
  int ax = SIDEBAR_W, ay = TOPBAR_H, aw = WW - ax, ah = WH - ay;
  DrawRectangle(ax, ay, aw, ah, CB_BG);

  if (gSubScr == 0) {
    InputField((Rectangle){ax + 14, ay + 12, 280, 36}, 0,
               "Tìm kiếm nhân viên...", 13);
    if (Button((Rectangle){WW - 206, ay + 12, 190, 36}, "+ Thêm nhân viên",
               CA_GOLD_DIM, CA_GOLD, 0)) {
      gSubScr = 1;
      memset(gInp, 0, sizeof(gInp));
      memset(gInpLen, 0, sizeof(gInpLen));
      memset(gInpPass, 0, sizeof(gInpPass));
    }

    int hY = ay + 56;
    DrawRectangle(ax, hY, aw, 40, CB_PANEL);
    struct {
      const char *l;
      int x;
    } hcols[] = {{"ID", ax + 10},         {"Tài khoản", ax + 64},
                 {"Họ và Tên", ax + 210}, {"Chức vụ", ax + 440},
                 {"Lương/h", ax + 620},   {"", ax + 760}};
    for (int i = 0; i < 6; i++)
      DrawTextEx(gFontB, hcols[i].l, (Vector2){hcols[i].x, hY + 12}, 14.f, 1.f,
                 CT_MUTED);
    DrawRectangle(ax, hY + 40, aw, 1, CB_BORDER);

    int rowH = 80, startY = hY + 42, visH = ah - 56 - 42;
    Rectangle la = {(float)ax, (float)startY, (float)aw, (float)visH};
    updateScroll(la, (float)(gStaffCount * rowH));

    BeginScissorMode(ax, startY, aw, visH);
    int vi = 0;
    for (int i = 0; i < gStaffCount; i++) {
      if (gInpLen[0] > 0 && !matchSearch(gInp[0], gStaff[i].name))
        continue;
      float fy = (float)(startY + vi * rowH) - gScrollY;
      vi++;
      if (fy + rowH < startY || fy > startY + visH)
        continue;
      Color rbg = (vi % 2 == 0) ? CB_ROW_ALT : CB_BG;
      DrawRectangle(ax, (int)fy, aw, rowH, rbg);
      DrawLine(ax, (int)(fy + rowH - 1), ax + aw, (int)(fy + rowH - 1),
               CB_BORDER);
      float ty = fy + (rowH - 18) * .5f;
      char ids[8];
      sprintf(ids, "%d", gStaff[i].id);
      DrawTxtL(ids, ax + 10, ty, 18.f, CT_DIM);
      DrawTxtL(gStaff[i].username, ax + 64, ty, 18.f, CT_MUTED);
      DrawTxtL(gStaff[i].name, ax + 210, ty, 18.f, CT_WHITE);
      DrawTxtL(gStaff[i].position, ax + 440, ty, 18.f, CT_MUTED);
      char sh[24];
      sprintf(sh, "%.0f đ/h", gStaff[i].salaryPerHour);
      DrawTxtL(sh, ax + 620, ty, 18.f, CA_GOLD);
      int by = (int)(fy + (rowH - 26) * .5f);
      if (gStaff[i].id != 100) {
        if (SmallBtn((Rectangle){ax + 756, by, 52, 26}, "Sửa", CB_CARD,
                     CB_CARD_HOV, 20 + i * 2)) {
          gSubScr = 2;
          gEditId = gStaff[i].id;
          strncpy(gInp[0], gStaff[i].username, 1019);
          gInpLen[0] = (int)strlen(gInp[0]);
          strncpy(gInp[2], gStaff[i].name, 1019);
          gInpLen[2] = (int)strlen(gInp[2]);
          strncpy(gInp[3], gStaff[i].position, 1019);
          gInpLen[3] = (int)strlen(gInp[3]);
          char sh2[24];
          sprintf(sh2, "%.0f", gStaff[i].salaryPerHour);
          strncpy(gInp[4], sh2, 1019);
          gInpLen[4] = (int)strlen(gInp[4]);
        }
        if (SmallBtn((Rectangle){ax + 812, by, 52, 26}, "Xóa",
                     (Color){255, 120, 160, 255}, CS_ERR, 21 + i * 2)) {
          openDialog("Xác nhận xóa", "Bạn có chắc muốn xóa nhân viên này?",
                     200 + i);
        }
      } else {
        DrawTxtL("[Admin]", ax + 762, ty, 16.f, CT_DIM);
      }
    }
    EndScissorMode();
  } else {
    bool isAdd = (gSubScr == 1);
    int fw = 600, fh = isAdd ? 500 : 430;
    int fx = (WW - SIDEBAR_W - fw) / 2 + SIDEBAR_W, fy = ay + 60;
    DrawRectangleRounded((Rectangle){fx, fy, fw, fh}, 0.16f, 12, CB_PANEL);
    DrawRectangleRoundedLines((Rectangle){fx, fy, fw, fh}, 0.16f, 12,
                              CB_BORDER);
    const char *ft = isAdd ? "THÊM NHÂN VIÊN MỚI" : "SỬA THÔNG TIN NHÂN VIÊN";
    Vector2 ftv = MeasureB(ft, 16);
    DrawTextEx(gFontB, ft, (Vector2){fx + (fw - ftv.x) * .5f, fy + 14}, 16.f,
               1.f, CA_GOLD);
    DrawLine(fx + 20, fy + 46, fx + fw - 20, fy + 46, CB_BORDER);

    int px = fx + 28, pw = fw - 56, row = 50;
    int yy = fy + 54;
    DrawTxtL("Tên đăng nhập:", px, yy, 16.f, CT_MUTED);
    InputField((Rectangle){px, yy + 18, pw, 42}, 0, "username...", 14);
    yy += row + 10;
    if (isAdd) {
      DrawTxtL("Mật khẩu:", px, yy, 16.f, CT_MUTED);
      gInpPass[1] = true;
      InputField((Rectangle){px, yy + 18, pw, 42}, 1, "password...", 14);
      yy += row + 10;
    }
    DrawTxtL("Họ và Tên:", px, yy, 16.f, CT_MUTED);
    InputField((Rectangle){px, yy + 18, pw, 42}, 2, "Tên đầy đủ...", 14);
    yy += row + 10;
    DrawTxtL("Chức vụ:", px, yy, 16.f, CT_MUTED);
    InputField((Rectangle){px, yy + 18, pw, 42}, 3, "Nhân viên / Quản lý...",
               14);
    yy += row + 10;
    DrawTxtL("Lương/giờ (VND):", px, yy, 16.f, CT_MUTED);
    InputField((Rectangle){px, yy + 18, pw, 42}, 4, "VD: 30000", 14);
    yy += row + 18;

    bool sv = Button((Rectangle){px, yy, (float)(pw / 2 - 8), 44}, "[S]  Lưu",
                     CA_GOLD_DIM, CA_GOLD, 1);
    bool cl = Button((Rectangle){px + pw / 2 + 8, yy, (float)(pw / 2 - 8), 44},
                     "[X]  Hủy", CB_CARD, CB_CARD_HOV, 2);
    if (cl) {
      gSubScr = 0;
      memset(gInp, 0, sizeof(gInp));
      memset(gInpLen, 0, sizeof(gInpLen));
    }
    if (sv) {
      bool ok = gInpLen[0] && gInpLen[2] && gInpLen[3];
      if (isAdd)
        ok = ok && gInpLen[1];
      if (!ok)
        showToast("Vui lòng điền đầy đủ thông tin!", CS_WARN);
      else {
        if (isAdd) {
          if (gStaffCount >= MAX_STAFF)
            showToast("Danh sách đã đầy!", CS_ERR);
          else {
            int nid = gStaffCount ? gStaff[gStaffCount - 1].id + 1 : 101;
            memset(&gStaff[gStaffCount], 0, sizeof(Staff));
            gStaff[gStaffCount].id = nid;
            strncpy(gStaff[gStaffCount].username, gInp[0], SHORT_STR - 1);
            strncpy(gStaff[gStaffCount].password, gInp[1], SHORT_STR - 1);
            strncpy(gStaff[gStaffCount].name, gInp[2], STR_LEN - 1);
            strncpy(gStaff[gStaffCount].position, gInp[3], SHORT_STR - 1);
            gStaff[gStaffCount].salaryPerHour = atof(gInp[4]);
            gStaffCount++;
            saveStaff();
            showToast("Đã thêm nhân viên thành công!", CS_OK);
            gSubScr = 0;
          }
        } else {
          for (int j = 0; j < gStaffCount; j++)
            if (gStaff[j].id == gEditId) {
              strncpy(gStaff[j].name, gInp[2], STR_LEN - 1);
              strncpy(gStaff[j].position, gInp[3], SHORT_STR - 1);
              gStaff[j].salaryPerHour = atof(gInp[4]);
              saveStaff();
              showToast("Đã cập nhật nhân viên!", CS_OK);
              gSubScr = 0;
              break;
            }
        }
      }
    }
  }
}

/* ═══════════════════════════════════════════════════════════════════
   THỐNG KÊ
═══════════════════════════════════════════════════════════════════ */
static void drawStats(void) {
  drawSidebar();
  drawTopBar("THỐNG KÊ DOANH THU & NHÂN SỰ");
  int ax = SIDEBAR_W, ay = TOPBAR_H, aw = WW - ax, ah = WH - ay;
  DrawRectangle(ax, ay, aw, ah, CB_BG);

  int occ = 0;
  float liveBill = 0;
  for (int i = 0; i < MAX_TABLES; i++)
    if (gTables[i].isOccupied) {
      occ++;
      liveBill += gTables[i].currentBill;
    }

  /* KPI cards */
  struct {
    const char *lbl, *sub;
    float val;
    const char *unit;
    Color col;
  } kpis[] = {
      {"Tổng Doanh Thu", "Đã thanh toán", gTotalRevenue, "  VND", CA_GOLD},
      {"Tạm Thu Hiện Tại", "Đang phục vụ", liveBill, "  VND", CS_OK},
      {"Bàn Đang Dùng", "Trong 10 bàn", (float)occ, " / 10", CS_INFO},
  };
  int kw = (aw - 64) / 3, kh = 120;
  for (int i = 0; i < 3; i++) {
    Rectangle kr = {(float)(ax + 16 + i * (kw + 16)), (float)(ay + 14),
                    (float)kw, (float)kh};
    DrawRectangleRounded(kr, 0.1f, 8, CB_PANEL);
    DrawRectangle((int)kr.x, (int)kr.y, 4, kh, kpis[i].col);
    DrawTxtL(kpis[i].lbl, kr.x + 18, kr.y + 16, 17.f, CT_MUTED);
    char vs[48];
    sprintf(vs, "%.0f", kpis[i].val);
    char full[64];
    sprintf(full, "%s%s", vs, kpis[i].unit);
    DrawTextEx(gFontB, full, (Vector2){kr.x + 18, kr.y + 46}, 22.f, 1.f,
               kpis[i].col);
    DrawTxtL(kpis[i].sub, kr.x + 16, kr.y + 72, 15.f, CT_DIM);
  }

  /* Trạng thái bàn */
  int gridY = ay + 128;
  DrawRectangle(ax + 14, gridY, aw - 28, 32, CB_PANEL);
  DrawTxtBL("TRẠNG THÁI CÁC BÀN", ax + 24, gridY + 10, 17.f, CT_MUTED);
  gridY += 34;
  int tcols = 5, tw2 = (aw - 28) / tcols, th3 = 100;
  for (int i = 0; i < MAX_TABLES; i++) {
    int col = i % tcols, row = i / tcols;
    Rectangle tr = {(float)(ax + 14 + col * tw2),
                    (float)(gridY + row * (th3 + 4)), (float)(tw2 - 4),
                    (float)th3};
    bool occ2 = gTables[i].isOccupied;
    Color tbg =
        occ2 ? (Color){255, 225, 235, 255} : (Color){225, 245, 235, 255};
    DrawRectangleRounded(tr, 0.12f, 6, tbg);
    Color sc2 = occ2 ? CS_ERR : CS_OK;
    DrawRectangleRoundedLines(tr, 0.12f, 6, sc2);
    char tn[16];
    sprintf(tn, "Bàn %02d", gTables[i].id);
    Vector2 tnv = Measure(tn, 16.f);
    DrawTxtL(tn, tr.x + (tr.width - tnv.x) * .5f, tr.y + 6, 16.f, CT_WHITE);
    if (occ2) {
      char bs[24];
      sprintf(bs, "%.0f đ", gTables[i].currentBill);
      Vector2 bv = MeasureB(bs, 18.f);
      DrawTextEx(gFontB, bs,
                 (Vector2){tr.x + (tr.width - bv.x) * .5f, tr.y + 36}, 18.f,
                 1.f, CA_GOLD);
    } else {
      Vector2 fv = Measure("Trống", 16.f);
      DrawTxtL("Trống", tr.x + (tr.width - fv.x) * .5f, tr.y + 36, 16.f, sc2);
    }
  }

  /* Lịch sử */
  int histY = gridY + 2 * (th3 + 4) + 10;
  DrawRectangle(ax + 14, histY, aw - 28, 32, CB_PANEL);
  DrawTxtBL("LỊCH SỬ HÓA ĐƠN GẦN ĐÂY", ax + 24, histY + 10, 17.f, CT_MUTED);
  histY += 34;
  FILE *fr = fopen("tongdoanhthu.txt", "r");
  if (fr) {
    char line[256];
    int li = 0;
    while (fgets(line, 256, fr) && li < 7) {
      line[strcspn(line, "\n")] = 0;
      Color lbg = (li % 2 == 0) ? CB_PANEL : CB_ROW_ALT;
      DrawRectangle(ax + 14, histY + li * 40, aw - 28, 39, lbg);
      DrawTxtL(line, ax + 24, histY + li * 40 + 10, 19.f, CT_WHITE);
      li++;
    }
    fclose(fr);
  } else {
    DrawTxtL("Chưa có dữ liệu hóa đơn.", ax + 24, histY + 10, 16.f, CT_DIM);
  }
}

/* ═══════════════════════════════════════════════════════════════════
   BIỂU ĐỒ DOANH THU
═══════════════════════════════════════════════════════════════════ */
static void drawChart(void) {
  drawSidebar();
  drawTopBar("BIỂU ĐỒ DOANH THU THEO NGÀY");
  int ax = SIDEBAR_W, ay = TOPBAR_H, aw = WW - ax, ah = WH - ay;
  DrawRectangle(ax, ay, aw, ah, CB_BG);

  typedef struct {
    char date[15];
    float total;
  } DayRev;
  DayRev data[30];
  int dc = 0;
  FILE *fp = fopen("tongdoanhthu.txt", "r");
  if (fp) {
    char line[256];
    while (fgets(line, 256, fp)) {
      /* Lấy ngày: 10 ký tự đầu dạng DD/MM/YYYY */
      char dt[15] = {0};
      if (strlen(line) < 10)
        continue;
      strncpy(dt, line, 10);
      dt[10] = 0;
      /* Kiểm tra định dạng ngày DD/MM/YYYY */
      if (dt[2] != '/' || dt[5] != '/')
        continue;
      /* Tìm số tiền: tìm chuỗi con ": " rồi đọc số sau đó */
      char *colon = strrchr(line, ':');
      if (!colon)
        continue;
      float amt = 0;
      if (sscanf(colon + 1, " %f", &amt) != 1)
        continue;
      bool found3 = false;
      for (int i = 0; i < dc; i++)
        if (strcmp(data[i].date, dt) == 0) {
          data[i].total += amt;
          found3 = true;
          break;
        }
      if (!found3 && dc < 30) {
        strcpy(data[dc].date, dt);
        data[dc].total = amt;
        dc++;
      }
    }
    fclose(fp);
  }
  if (!dc) {
    Vector2 nv = Measure("Chua co du lieu de ve bieu do.", 16);
    DrawTxtL("Chua co du lieu de ve bieu do.", ax + (aw - nv.x) * .5f,
             ay + ah * .5f - 8, 16.f, CT_DIM);
    return;
  }

  /* Tính tổng toàn bộ từ file (không phụ thuộc gTotalRevenue runtime) */
  float totalAllTime = 0;
  float maxRev = 0;
  for (int i = 0; i < dc; i++) {
    totalAllTime += data[i].total;
    if (data[i].total > maxRev)
      maxRev = data[i].total;
  }
  if (maxRev < 1)
    maxRev = 1;

  /* Chỉ hiện tối đa 10 ngày gần nhất để tránh nhãn đè lên nhau */
  int maxBars = 10;
  int startBar = 0;
  if (dc > maxBars)
    startBar = dc - maxBars;
  int visibleDc = dc - startBar;

  int chartX = ax + 70, chartY = ay + 40, chartW = aw - 110, chartH = ah - 150;

  /* Lưới ngang */
  for (int g = 0; g <= 5; g++) {
    int gy = chartY + chartH - g * (chartH / 5);
    DrawLine(chartX, gy, chartX + chartW, gy, CB_BORDER);
    char lbl[32];
    sprintf(lbl, "%.0f", g * (maxRev / 5.f));
    Vector2 lv = Measure(lbl, 13);
    DrawTxtL(lbl, chartX - lv.x - 6, gy - 7, 13.f, CT_DIM);
  }

  /* Cột */
  int bw = fmaxf(24, (chartW - visibleDc * 10) / visibleDc);
  int bg2 = (chartW - visibleDc * bw) / (visibleDc + 1);
  for (int i = 0; i < visibleDc; i++) {
    int di = startBar + i; /* index thực trong mảng data */
    int bx = chartX + bg2 + i * (bw + bg2);
    int bh = (int)((data[di].total / maxRev) * chartH);
    int by = chartY + chartH - bh;

    /* gradient cột */
    for (int h2 = 0; h2 < bh; h2++) {
      float r = (float)h2 / bh;
      Color bc = {
          (unsigned char)(CA_GOLD_DIM.r + (CA_GOLD.r - CA_GOLD_DIM.r) * r),
          (unsigned char)(CA_GOLD_DIM.g + (CA_GOLD.g - CA_GOLD_DIM.g) * r),
          (unsigned char)(CA_GOLD_DIM.b + (CA_GOLD.b - CA_GOLD_DIM.b) * r),
          215};
      DrawRectangle(bx, by + bh - h2, bw, 1, bc);
    }
    /* glow đỉnh */
    DrawRectangle(bx, by, bw, 4, CA_GOLD_LITE);

    /* nhãn ngày — xoay 45 độ nếu cột hẹp */
    int labelX = bx + bw / 2;
    int labelY = chartY + chartH + 10;
    if (bw >= 60) {
      /* đủ rộng: vẽ ngang */
      Vector2 dv = Measure(data[di].date, 12);
      DrawTxtL(data[di].date, labelX - (int)(dv.x * .5f), labelY, 12.f,
               CT_MUTED);
    } else {
      /* hẹp: vẽ xoay 45 độ */
      DrawTextPro(gFont, data[di].date, (Vector2){(float)labelX, (float)labelY},
                  (Vector2){0, 0}, 45.f, 12.f, 1.f, CT_MUTED);
    }

    /* giá trị trên đỉnh */
    if (bh > 20) {
      char vs[24];
      sprintf(vs, "%.0f", data[di].total);
      Vector2 vv = Measure(vs, 12);
      DrawTxtL(vs, bx + (bw - (int)vv.x) / 2, by - 16, 12.f, CA_GOLD_LITE);
    }

    /* tooltip hover */
    Rectangle br = {(float)bx, (float)by, (float)bw, (float)bh};
    if (CheckCollisionPointRec(GetMousePosition(), br)) {
      DrawRectangleRounded((Rectangle){bx - 18, by - 52, 148, 44}, 0.2f, 6,
                           CB_PANEL);
      DrawRectangleRoundedLines((Rectangle){bx - 18, by - 52, 148, 44}, 0.2f, 6,
                                CB_BORDER2);
      DrawTxtL(data[di].date, bx - 14, by - 48, 13.f, CT_MUTED);
      char amt[32];
      sprintf(amt, "%.0f VND", data[di].total);
      DrawTextEx(gFontB, amt, (Vector2){bx - 14, by - 32}, 15.f, 1.f, CA_GOLD);
    }
  }

  /* Ghi chú nếu có nhiều ngày hơn đang hiện */
  if (dc > maxBars) {
    char note[64];
    sprintf(note, "(Hien %d/%d ngay gan nhat)", visibleDc, dc);
    Vector2 nv2 = Measure(note, 13);
    DrawTxtL(note, ax + (aw - nv2.x) * .5f, chartY - 20, 13.f, CT_DIM);
  }

  /* Trục */
  DrawLine(chartX, chartY, chartX, chartY + chartH, CB_BORDER2);
  DrawLine(chartX, chartY + chartH, chartX + chartW, chartY + chartH,
           CB_BORDER2);

  /* Tổng */
  char sumStr[64];
  sprintf(sumStr, "Tong tat ca:  %.0f VND", totalAllTime);
  Vector2 sv4 = MeasureB(sumStr, 17);
  DrawTextEx(gFontB, sumStr,
             (Vector2){ax + (aw - sv4.x) * .5f, chartY + chartH + 70}, 17.f,
             1.f, CA_GOLD);
}

/* ═══════════════════════════════════════════════════════════════════
   DIALOG XÁC NHẬN
═══════════════════════════════════════════════════════════════════ */
static void drawDialog(void) {
  if (!gDlgOpen)
    return;

  /* Overlay mờ */
  DrawRectangle(0, 0, WW, WH, (Color){0, 0, 0, 155});

  int dw = 520, dh = 260;
  int dx = (WW - dw) / 2, dy = (WH - dh) / 2;

  DrawRectangleRounded((Rectangle){dx + 6, dy + 8, dw, dh}, 0.08f, 10,
                       (Color){0, 0, 0, 100});
  DrawRectangleRounded((Rectangle){dx, dy, dw, dh}, 0.08f, 10, CB_PANEL);
  DrawRectangleRoundedLines((Rectangle){dx, dy, dw, dh}, 0.08f, 10, CB_BORDER2);

  Vector2 tv = MeasureB(gDlgTitle, 19);
  DrawTextEx(gFontB, gDlgTitle, (Vector2){dx + (dw - tv.x) * .5f, dy + 18},
             19.f, 1.f, CA_GOLD);
  DrawLine(dx + 20, dy + 54, dx + dw - 20, dy + 54, CB_BORDER);
  Vector2 mv = Measure(gDlgBody, 16);
  DrawTxtL(gDlgBody, dx + (dw - mv.x) * .5f, dy + 70, 16.f, CT_WHITE);

  Color yesBg = (Color){255, 120, 160, 255};
  Color yesHov = (Color){255, 90, 140, 255};
  Color noBg = (Color){210, 225, 255, 255};
  Color noHov = (Color){180, 205, 255, 255};

  bool yes = Button((Rectangle){dx + 40, dy + 160, (float)(dw / 2 - 54), 52},
                    "[OK]  Có, xóa ngay", yesBg, yesHov, 80);
  bool no =
      Button((Rectangle){dx + dw / 2 + 14, dy + 160, (float)(dw / 2 - 54), 52},
             "[X]  Không", noBg, noHov, 81);

  if (no || IsKeyPressed(KEY_ESCAPE)) {
    gDlgOpen = false;
    return;
  }

  if (yes) {
    gDlgOpen = false;
    int act = gDlgAction;
    if (act >= 100 && act < 200) { /* xóa menu */
      int idx = act - 100;
      if (idx >= 0 && idx < gMenuCount) {
        for (int j = idx; j < gMenuCount - 1; j++)
          gMenu[j] = gMenu[j + 1];
        memset(&gMenu[gMenuCount - 1], 0, sizeof(MenuItem));
        gMenuCount--;
        saveMenu();
        showToast("Đã xóa món thành công!", CS_OK);
      }
    } else if (act >= 200 && act < 300) { /* xóa staff */
      int idx = act - 200;
      if (idx >= 0 && idx < gStaffCount && gStaff[idx].id != 100) {
        for (int j = idx; j < gStaffCount - 1; j++)
          gStaff[j] = gStaff[j + 1];
        memset(&gStaff[gStaffCount - 1], 0, sizeof(Staff));
        gStaffCount--;
        saveStaff();
        showToast("Đã xóa nhân viên!", CS_OK);
      }
    } else if (act >= 300 && act < 400) { /* xóa đơn bàn */
      int tidx = act - 300;
      gTables[tidx].isOccupied = 0;
      gTables[tidx].currentBill = 0;
      gTables[tidx].itemCount = 0;
      memset(gTables[tidx].items, 0, sizeof(gTables[tidx].items));
      showToast("Đã làm mới đơn!", CS_INFO);
      resetScreen(SCR_TABLES);
    }
  }
}

/* ═══════════════════════════════════════════════════════════════════
   TOAST
═══════════════════════════════════════════════════════════════════ */
static void drawToast(void) {
  if (!gToastActive)
    return;
  gToastTimer -= GetFrameTime();
  if (gToastTimer <= 0) {
    gToastActive = false;
    return;
  }
  float alpha = fminf(1.f, gToastTimer / 0.5f) *
                fminf(1.f, (3.f - gToastTimer + 0.5f) / 0.5f);
  int tw = 500, th = 52;
  int tx = (WW - tw) / 2, ty = WH - th - 22;
  DrawRectangleRounded((Rectangle){tx + 4, ty + 4, tw, th}, 0.45f, 10,
                       (Color){0, 0, 0, (unsigned char)(70 * alpha)});
  Color bg = {gToastColor.r, gToastColor.g, gToastColor.b,
              (unsigned char)(215 * alpha)};
  DrawRectangleRounded((Rectangle){tx, ty, tw, th}, 0.45f, 10, bg);
  Vector2 mv = Measure(gToastMsg, 17);
  DrawTxtL(gToastMsg, tx + (tw - mv.x) * .5f, ty + (th - 17) * .5f, 17.f,
           (Color){255, 255, 255, (unsigned char)(255 * alpha)});
}

/* ═══════════════════════════════════════════════════════════════════
   MAIN
═══════════════════════════════════════════════════════════════════ */
int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(WIN_W, WIN_H, "ITF COFFEE - Hệ Thống Quản Lý Quán Cà Phê");
  SetTargetFPS(FPS);
  SetExitKey(KEY_NULL);

  /* Tải font tiếng Việt — thứ tự ưu tiên */
  const char *regularPaths[] = {
      "./BeVietnamPro-Regular.ttf", "./fonts/BeVietnamPro-Regular.ttf",
      /* Noto Sans — ho tro tieng Viet day du, co san tren Linux/macOS/Windows
       */
      "./NotoSans-Regular.ttf", "./fonts/NotoSans-Regular.ttf",
      "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
      "/usr/share/fonts/noto/NotoSans-Regular.ttf",
      "/System/Library/Fonts/Supplemental/NotoSans-Regular.ttf",
      /* Windows fallback */
      "C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/tahoma.ttf",
      /* Linux fallback */
      "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
      "/usr/share/fonts/truetype/freefont/FreeSans.ttf", NULL};
  const char *boldPaths[] = {
      "./BeVietnamPro-Bold.ttf", "./fonts/BeVietnamPro-Bold.ttf",
      /* Noto Sans Bold */
      "./NotoSans-Bold.ttf", "./fonts/NotoSans-Bold.ttf",
      "/usr/share/fonts/truetype/noto/NotoSans-Bold.ttf",
      "/usr/share/fonts/noto/NotoSans-Bold.ttf",
      "/System/Library/Fonts/Supplemental/NotoSans-Bold.ttf",
      /* Windows fallback */
      "C:/Windows/Fonts/segoeuib.ttf", "C:/Windows/Fonts/tahomabd.ttf",
      /* Linux fallback */
      "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
      "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", NULL};

  gFont = tryLoadFont(regularPaths, 33);
  gFontB = tryLoadFont(boldPaths, 33);
  if (gFontB.baseSize == 0)
    gFontB = gFont;

  /* Khởi tạo dữ liệu */
  loadStaff();
  loadMenu();
  for (int i = 0; i < MAX_TABLES; i++) {
    gTables[i].id = i + 1;
    gTables[i].isOccupied = 0;
    gTables[i].itemCount = 0;
    gTables[i].currentBill = 0;
  }

  gScreen = SCR_LOGIN;
  gActiveInp = -1;
  memset(gHov, 0, sizeof(gHov));

  /* ── Vòng lặp chính ── */
  while (!WindowShouldClose()) {
    gElapsed += GetFrameTime();

    /* Chỉ xử lý input khi không có dialog */
    if (!gDlgOpen)
      processInput();

    BeginDrawing();
    ClearBackground(CB_BG);

    switch (gScreen) {
    case SCR_LOGIN:
      drawLogin();
      break;
    case SCR_TABLES:
      drawTableMap();
      break;
    case SCR_ORDER:
      drawOrder();
      break;
    case SCR_INVOICE:
      drawInvoice();
      break;
    case SCR_MENU:
      drawMenuManage();
      break;
    case SCR_STAFF:
      drawStaffManage();
      break;
    case SCR_STATS:
      drawStats();
      break;
    case SCR_CHART:
      drawChart();
      break;
    }

    /* Dialog & Toast luôn vẽ sau cùng (on top) */
    drawDialog();
    drawToast();

    EndDrawing();
  }

  /* Giải phóng tài nguyên */
  if (gFontB.baseSize != gFont.baseSize)
    UnloadFont(gFontB);
  UnloadFont(gFont);
  CloseWindow();
  return 0;
}
