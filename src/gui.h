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

typedef struct gui_glyph_metric
{
    s32 MinX;
    s32 MaxX;
    s32 MinY;
    s32 MaxY;
    s32 Advance;
} gui_glyph_metric;

void DisplayWindow(void);
