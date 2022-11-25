typedef struct gui_cursor
{
    u32 BufferIndex;
    u32 X;
    u32 Y;
} gui_cursor;

typedef struct gui_state
{
    u32 Running;
    u32 FontSize;
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
