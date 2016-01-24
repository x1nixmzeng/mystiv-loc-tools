# Myst IV Locale Tools

This project is a collection of translation tools for Myst IV.

For general game format discussions visit [XeNTaX][1].


## Usage

### M4B (Myst IV Binary file)

This is a packed data container. Use `myst_m4b.exe`:

    Usage:
     List files    m4b l file.m4b
     Extract files m4b e file.m4b folder
     Pack folder   m4b p folder file.m4b

### BIN (Binary file)

This is a serialized binary file used by Myst IV. Use `myst_loc.exe`:

    Usage:
     myst_loc f <file>.bin <result>.xml
     myst_loc t <file>.xml <result>.bin

| Type     | Supported |
|----------|:---------:|
| Subtitle |      ✓   |
| Text     |      ✓   |


## Licence

This code has been placed in the public domain

[1]:http://forum.xentax.com/viewforum.php?f=10
