typedef struct gui_cursor
{
    u32 BufferIndex;
    u32 X;
    u32 Y;
} gui_cursor;

typedef struct gui_font_data
{
    u32 Size;
    s32 Ascent;
    s32 Descent;
    s32 Height;
} gui_font_data;

typedef struct gui_state
{
    u32 Running;
    gui_font_data FontData;
    gui_cursor Cursor;
} gui_state;

typedef struct gui_stb_bitmap
{
    s32 Width;
    s32 Height;
    u8 *At;
} gui_stb_bitmap;

typedef struct gui_rect
{
    s32 X0;
    s32 Y0;
    s32 X1;
    s32 Y1;
} gui_rect;

typedef struct gui_char_data
{
    s32 X0;
    s32 Y0;
    s32 X1;
    s32 Y1;
    s32 XOffset;
    s32 YOffset;
} gui_char_data;


void DisplayWindow(void);
