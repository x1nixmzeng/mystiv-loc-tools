# Myst IV Locale Tools

This project is a collection of translation tools for Myst IV.

For general game format discussions visit [XeNTaX][1].


## Tools

### M4B (Myst IV Binary file)

This is a packed data container. Use `myst_m4b.exe`:

    Usage:
     List files    myst_m4b l file.m4b
     Extract files myst_m4b e file.m4b folder
     Pack folder   myst_m4b p folder file.m4b

To compile, run this the batch script `compile.bat`.


### BIN (Binary file)

This is a serialized binary file used by Myst IV. This table lists the supported tools:

| Type     | Extract | Repack | Tool               | Build script  |
|----------|:-------:|:------:|--------------------|---------------|
| Subtitle |    ✓   |    ✓   | `myst_loc`         | `compile_bin` |
| Text     |    ✓   |    ✓   | `myst_loc`         | `compile_bin` |
| Texture  |    ✓   |         | `myst_extract_img` | `compile_img` |

* For localisation, use `myst_loc.exe`:

    Usage:
     myst_loc f <file>.bin <result>.xml
     myst_loc t <file>.xml <result>.bin

* For textures, use `myst_extract_img.exe`:

    Usage:
     myst_extract_img e <file>.bin <result>


## Known Usage

* [Turkish Myst IV Translation][2]


## Licence

This code has been placed in the public domain

[1]:http://forum.xentax.com/viewforum.php?f=10
[2]:http://www.tanersaydam.com/myst-iv-revelation-0-turkce-yamasi-kurulumda/
