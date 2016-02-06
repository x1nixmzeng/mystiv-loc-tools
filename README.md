# Myst IV Tools

This project is a collection of tools to translate Myst IV.

For general game format discussions visit [XeNTaX][1].


## Tools

### M4B Files

This tool extracts and repacks the contents of the binary data container used by Myst IV.

Use the tool `myst_m4b.exe`. Usage:


     List files    myst_m4b l file.m4b
     Extract files myst_m4b e file.m4b folder
     Pack folder   myst_m4b p folder file.m4b


To compile, run this the batch script `compile.bat`.


### BIN Files

Binary files contain different types of content by by Myst IV. This table lists the tools:

| Type     | Extract | Repack | Tool               | Build script  |
|----------|:-------:|:------:|--------------------|---------------|
| Subtitle |    ✓   |    ✓   | `myst_loc`         | `compile_bin` |
| Text     |    ✓   |    ✓   | `myst_loc`         | `compile_bin` |
| Texture  |    ✓   |         | `myst_extract_img` | `compile_img` |

 * For localisation, use `myst_loc.exe`:
 * For textures, use `myst_extract_img.exe`:


## Known Usage

 * [Turkish Myst IV Translation][2]


## Licence

This code has been placed in the public domain

[1]:http://forum.xentax.com/viewforum.php?f=10
[2]:http://www.tanersaydam.com/myst-iv-revelation-0-turkce-yamasi-kurulumda/
