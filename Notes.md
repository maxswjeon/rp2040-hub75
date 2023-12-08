```
------------------------      ------------------------
|                      |      |                      |
|                      | ---- |                      |
|                    . |      |                    . |
------------------------      ------------------------
            |
------------------------      ------------------------
| .                    |      | .                    |
|                      | ---- |                      | ---
|                      |      |                      |
------------------------      ------------------------
```

Panel Table
|Index|Min x|Max x|Min y|Max y|rotation|
|:---:|---|---|---|---|---|
|0 | 64| 127 | 0 | 31 | 180|
|1 | 0 | 63 | 0 | 31 | 180|
|2 | 0 | 63 | 32 | 63 | 0|
|3 | 64| 127 | 32 | 63 | 0|

```cpp
int min_x, max_x, min_y, max_y, cur_x, cur_y;
int rotation_scheme = config[0].rotation % 180;
int rotation_x = rotation_scheme == 0 ? width : height;
int rotation_y = rotation_scheme == 0 ? height : width;

for (int i = 0; i < count; ++i)
{
    int table_index = count - 1 - i;

    if (rotation_scheme != config[0].rotation % 180) {
        return;
    }

    switch (config[i].next_postion)
    {
    case LEFT:
        cur_x -= rotation_x;
        min_x = cur_x < min_x ? cur_x : min_x;
        break;
    case RIGHT:
        cur_x += rotation_x;
        max_x = cur_x > max_x ? cur_x : max_x;
        break;
    case UP:
        cur_y -= rotation_y;
        min_y = cur_y < min_y ? cur_y : min_y;
        break;
    case DOWN:
        cur_y += rotation_y;
        max_y = cur_y > max_y ? cur_y : max_y;
        break;
    }
    table[table_index].min_x = cur_x;
    table[table_index].max_x = cur_x + rotation_x;
    table[table_index].min_y = cur_y;
    table[table_index].max_y = cur_y + rotation_y;
}
```

Virtual Width 128
Virtual Height 64

```cpp
int panel_index = 0;
for (; panel_index < count; ++panel_index) {
    // Panel Table Hit
    if (_panel_table[i].min_x <= x && _panel_table[i].max_x >= x && _panel_table[i].min_y <= y&& _panel_table[i].max_y >= y) {
        break;
    }
}

// Out of Bounds
if (panel_index == count) {
    return;
}

int px, py;
// Rotation consistency is assumed - (0, 180), (90, 270) is paired
if (_panel_table[panel_index].rotation == 0) {
    px = panel_index * _width + (x % _width);
    py = (y % _height);
}
else if (_panel_table[panel_index].rotation == 180) {
    px = panel_index * _width + (_width - 1 - (x % _width));
    py = _height - 1 - (y % _height);
}
else if (_panel_table[panel_index].rotation == 90) {
    px = panel_index * _width + (_height - 1 - (y % _height));
    py = (x % _width);
}
else if (_panel_table[panel_index].rotation == 270) {
    px = panel_index * _width + (y % _height);
    py = _width - 1 - (x % _width);
}
```
