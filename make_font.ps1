# Generates assets/font_digits.png: Game Build Phase 3's bitmap font atlas.
# ONE ROW of 11 fixed 16x16 cells — index 0..9 = digits '0'..'9',
# index 10 = the decimal point '.' — white glyphs on a TRANSPARENT
# background, 176x16 RGBA total.
#
# Every glyph is a hand-coded 5x7 dot-matrix pattern defined right here
# in the script, scaled 2x to 10x14 and placed at cell offset (3, 1).
# No system fonts, no rendering libraries: the EXACT pixels are data in
# this file, so the atlas is byte-for-byte reproducible on any machine —
# the same discipline as make_checker.ps1 and make_beep.ps1. The PNG is
# likewise written by hand: signature, IHDR, zlib-wrapped deflate IDAT,
# IEND — big-endian fields, CRC32 per chunk. The ONLY byte that differs
# from make_checker.ps1 is IHDR's color type: 6 (RGBA) instead of 2
# (RGB), because text needs per-pixel ALPHA so glyph quads blend over
# the scene instead of showing a solid backing rectangle.
$ErrorActionPreference = 'Stop'
$assetDir = Join-Path 'd:\PureEngine' 'assets'
if (-not (Test-Path $assetDir)) { New-Item -ItemType Directory -Path $assetDir | Out-Null }
$out = Join-Path $assetDir 'font_digits.png'

$cell = 16; $cols = 11
$width = $cell * $cols; $height = $cell     # 176 x 16

# --- CRC32 (PNG spec polynomial 0xEDB88320, reflected) ---
$crcTable = New-Object uint32[] 256
for ($n = 0; $n -lt 256; $n++) {
    $c = [uint32]$n
    for ($k = 0; $k -lt 8; $k++) {
        if ($c -band 1) { $c = 0xEDB88320 -bxor ($c -shr 1) } else { $c = $c -shr 1 }
    }
    $crcTable[$n] = $c
}
function Get-Crc32([byte[]]$data) {
    [int64]$c = 0xFFFFFFFF
    foreach ($b in $data) { $c = $crcTable[($c -bxor $b) -band 0xFF] -bxor ($c -shr 8) }
    return (($c -bxor 0xFFFFFFFF) -band 0xFFFFFFFF)
}
# Big-endian byte helpers
function Get-BE32([uint32]$v) { return [byte[]]@((($v -shr 24) -band 0xFF), (($v -shr 16) -band 0xFF), (($v -shr 8) -band 0xFF), ($v -band 0xFF)) }
function Get-Chunk([string]$type, [byte[]]$data) {
    $t = [System.Text.Encoding]::ASCII.GetBytes($type)
    $crc = Get-Crc32 ($t + $data)
    return (Get-BE32 ([uint32]$data.Length)) + $t + $data + (Get-BE32 $crc)
}

# --- The font itself: 11 glyphs, each seven 5-character rows ---
# '1' = glyph pixel, '0' = transparent. These patterns ARE the font.
$glyphs = @(
    ,@('01110','10001','10011','10101','11001','10001','01110')   # 0
    ,@('00100','01100','00100','00100','00100','00100','01110')   # 1
    ,@('01110','10001','00001','00010','00100','01000','11111')   # 2
    ,@('11111','00010','00100','00010','00001','10001','01110')   # 3
    ,@('00010','00110','01010','10010','11111','00010','00010')   # 4
    ,@('11111','10000','11110','00001','00001','10001','01110')   # 5
    ,@('00110','01000','10000','11110','10001','10001','01110')   # 6
    ,@('11111','00001','00010','00100','01000','01000','01000')   # 7
    ,@('01110','10001','10001','01110','10001','10001','01110')   # 8
    ,@('01110','10001','10001','01111','00001','00010','01100')   # 9
    ,@('00000','00000','00000','00000','00000','01100','01100')   # .
)

# --- Raw image: each scanline prefixed with filter byte 0, RGBA ---
$raw = New-Object byte[] ($height * (1 + $width * 4))
$idx = 0
for ($y = 0; $y -lt $height; $y++) {
    $raw[$idx] = 0; $idx++                     # filter: None
    for ($x = 0; $x -lt $width; $x++) {
        $g = [int][math]::Floor($x / $cell)    # which cell (glyph index)
        $cx = $x % $cell; $cy = $y
        # 2x scale of the 5x7 pattern, placed at cell offset (3, 1)
        $on = $false
        if ($cx -ge 3 -and $cx -lt 13 -and $cy -ge 1 -and $cy -lt 15) {
            $row = $glyphs[$g][[int][math]::Floor(($cy - 1) / 2)]
            $on = $row[[int][math]::Floor(($cx - 3) / 2)] -eq '1'
        }
        if ($on) { $raw[$idx] = 255; $raw[$idx + 1] = 255; $raw[$idx + 2] = 255; $raw[$idx + 3] = 255 }
        else     { $raw[$idx] = 0;   $raw[$idx + 1] = 0;   $raw[$idx + 2] = 0;   $raw[$idx + 3] = 0 }
        $idx += 4
    }
}

# --- zlib wrapper: header 0x78 0x01 + raw deflate + adler32 ---
$adlerA = [uint32]1; $adlerB = [uint32]0
foreach ($b in $raw) { $adlerA = ($adlerA + $b) % 65521; $adlerB = ($adlerB + $adlerA) % 65521 }
$adler = ($adlerB -shl 16) -bor $adlerA

$ms = New-Object System.IO.MemoryStream
$ms.WriteByte(0x78); $ms.WriteByte(0x01)
$deflate = New-Object System.IO.Compression.DeflateStream($ms, [System.IO.Compression.CompressionMode]::Compress, $true)
$deflate.Write($raw, 0, $raw.Length)
$deflate.Dispose()
$ms.Write((Get-BE32 $adler), 0, 4)
$idat = $ms.ToArray()
$ms.Dispose()

# --- Assemble PNG (color type 6 = RGBA — the one checker differs by) ---
$signature = [byte[]]@(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A)
$ihdr = (Get-BE32 $width) + (Get-BE32 $height) + [byte[]]@(8, 6, 0, 0, 0)  # 8-bit, RGBA, deflate, filter0, no interlace
$png = $signature + (Get-Chunk 'IHDR' $ihdr) + (Get-Chunk 'IDAT' $idat) + (Get-Chunk 'IEND' @())
[System.IO.File]::WriteAllBytes($out, $png)
Write-Host "Wrote $out ($($png.Length) bytes, ${width}x${height} RGBA, 11 cells: digits 0-9 + '.')"
