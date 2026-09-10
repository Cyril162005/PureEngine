# Step 59A: generates assets/paddle_spritesheet.png — 256x32 RGB, 8 frames
# of 32x32 cells for Pong-paddle animation proof (Step 59B wires it).
# Each cell is a distinct hue (red -> purple) with a darker 2px border so
# frame boundaries read at a glance. Same hand-written PNG machinery as
# make_textures.ps1: signature, IHDR, zlib-wrapped deflate IDAT, IEND —
# big-endian fields, CRC32 per chunk, filter byte 0 per scanline.
# Every pixel is data in THIS script: reproducible byte-for-byte.
# TINT RULE (inherited): every cell keeps red-channel content, so a future
# Step-8-style (1,0,0) collision tint can never render a frame BLACK.
$ErrorActionPreference = 'Stop'
$assetDir = Join-Path 'd:\PureEngine' 'assets'
if (-not (Test-Path $assetDir)) { New-Item -ItemType Directory -Path $assetDir | Out-Null }

$frames = 8; $cell = 32
$width = $frames * $cell; $height = $cell

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
function Get-BE32([uint32]$v) { return [byte[]]@((($v -shr 24) -band 0xFF), (($v -shr 16) -band 0xFF), (($v -shr 8) -band 0xFF), ($v -band 0xFF)) }
function Get-Chunk([string]$type, [byte[]]$data) {
    $t = [System.Text.Encoding]::ASCII.GetBytes($type)
    $crc = Get-Crc32 ($t + $data)
    return (Get-BE32 ([uint32]$data.Length)) + $t + $data + (Get-BE32 $crc)
}

# --- HSV hues, red -> purple, all with red-channel content (tint rule) ---
$hues = @(0, 25, 50, 110, 170, 220, 265, 300)
function Get-Rgb([int]$h) {
    $c = 255; $x = [int](255 * (1 - [Math]::Abs((($h / 60) % 2) - 1)))
    if ($h -lt 60) { return @(255, $x, 70) }
    if ($h -lt 120) { return @($x, 255, 70) }
    if ($h -lt 180) { return @(70, 255, $x) }
    if ($h -lt 240) { return @(70, $x, 255) }
    if ($h -lt 300) { return @($x, 70, 255) }
    return @(255, 70, $x)
}

$raw = New-Object byte[] ($height * (1 + $width * 3))
$idx = 0
for ($y = 0; $y -lt $height; $y++) {
    $raw[$idx] = 0; $idx++                     # filter: None
    for ($x = 0; $x -lt $width; $x++) {
        $f = [Math]::Floor($x / $cell)
        $lx = $x % $cell; $ly = $y
        if ($lx -lt 2 -or $lx -ge ($cell - 2) -or $ly -lt 2 -or $ly -ge ($height - 2)) {
            $px = @(25, 25, 25)                # dark border: cell edges read clearly
        } else {
            $px = Get-Rgb $hues[$f]
        }
        $raw[$idx] = $px[0]; $raw[$idx + 1] = $px[1]; $raw[$idx + 2] = $px[2]; $idx += 3
    }
}
# zlib wrapper: header 0x78 0x01 + raw deflate + adler32
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
$signature = [byte[]]@(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A)
$ihdr = (Get-BE32 $width) + (Get-BE32 $height) + [byte[]]@(8, 2, 0, 0, 0)
$png = $signature + (Get-Chunk 'IHDR' $ihdr) + (Get-Chunk 'IDAT' $idat) + (Get-Chunk 'IEND' @())
$out = Join-Path $assetDir 'paddle_spritesheet.png'
[System.IO.File]::WriteAllBytes($out, $png)
Write-Host "Wrote $out ($($png.Length) bytes, ${width}x${height} RGB, $frames frames)"
