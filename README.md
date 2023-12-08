# RP2040 HUB75 Panel Library

This is a HUB75 Panel Library, with PIO and DMA support and Virtual Screen with Chainning

## Virtual Screen

> **Info**
>
> Currently, constructing panels with different sizes are not supported yet.

### Configuration

Virtual Screen is constructed with same `Panel` class. The constructor should be called with those informations.

- Physical Panel Width
- Physical Panel Height
- Physical Panel Count
- `PanelConfig` array

**Important**
The order of the `PanelConfig` info should follow the wire, from the closest to the RP2040 to the end.

#### Examples

**Example 1**

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

```json
[
  {
    "rotation": 0,
    "next_position": "LEFT"
  },
  {
    "rotation": 0,
    "next_position": "UP"
  },
  {
    "rotation": 180,
    "next_position": "RIGHT"
  },
  {
    "rotation": 180,
    "next_position": "NONE"
  }
]
```
