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
    s32 Scale;
    u8 *At;
} gui_stb_bitmap;

void DisplayWindow(void);
