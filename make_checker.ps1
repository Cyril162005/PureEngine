# Generates assets/checker.png: 64x64 RGB PNG, 8x8-block checkerboard of
# orange (255,128,51) and deep blue (20,20,160) — matches the engine's
# Step 4 orange and Step 3 dark blue, so the texture is recognizably ours.
# PNG format is written by hand: signature, IHDR, zlib-wrapped deflate
# IDAT, IEND — every multi-byte field big-endian, CRC32 per chunk.
$ErrorActionPreference = 'Stop'
$assetDir = Join-Path 'd:\PureEngine' 'assets'
if (-not (Test-Path $assetDir)) { New-Item -ItemType Directory -Path $assetDir | Out-Null }
$out = Join-Path $assetDir 'checker.png'

$width = 64; $height = 64; $block = 8
$orange = @(255, 128, 51); $blue = @(20, 20, 160)

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

# --- Raw image: each scanline prefixed with filter byte 0 ---
$raw = New-Object byte[] ($height * (1 + $width * 3))
$idx = 0
for ($y = 0; $y -lt $height; $y++) {
    $raw[$idx] = 0; $idx++                     # filter: None
    for ($x = 0; $x -lt $width; $x++) {
        if (([int]([math]::Floor($x / $block)) + [int]([math]::Floor($y / $block))) % 2 -eq 0) { $px = $orange } else { $px = $blue }
        $raw[$idx] = $px[0]; $raw[$idx + 1] = $px[1]; $raw[$idx + 2] = $px[2]; $idx += 3
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

# --- Assemble PNG ---
$signature = [byte[]]@(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A)
$ihdr = (Get-BE32 $width) + (Get-BE32 $height) + [byte[]]@(8, 2, 0, 0, 0)  # 8-bit, RGB, deflate, filter0, no interlace
$png = $signature + (Get-Chunk 'IHDR' $ihdr) + (Get-Chunk 'IDAT' $idat) + (Get-Chunk 'IEND' @())
[System.IO.File]::WriteAllBytes($out, $png)
Write-Host "Wrote $out ($($png.Length) bytes, ${width}x${height} RGB checkerboard)"
