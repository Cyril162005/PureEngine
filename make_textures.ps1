# Game Build Phase 5: generates three 16x16 RGB PNGs in assets/ —
#   tex_player.png   warm green   — entity 0 (you)
#   tex_scenery.png  steel blue   — entities 1..2 (the scenery pair)
#   tex_hostile.png  crimson      — entities 3..5 (the threats)
# Same hand-written PNG machinery as make_checker.ps1: signature, IHDR,
# zlib-wrapped deflate IDAT, IEND — big-endian fields, CRC32 per chunk,
# filter byte 0 per scanline. Every pixel is data in THIS script, so the
# textures are reproducible byte-for-byte with no art tools.
# TINT RULE: every color keeps red-channel content. The Step 8 collision
# tint multiplies the sampled texel by (1,0,0) — a texture with zero red
# would render BLACK on collision. Red is kept in the palette so the
# danger feedback stays visible on every tintable entity (player and
# scenery; hostiles are never tinted — the colliding flags are only set
# inside the scenery loop — so crimson is free for them as the
# threat-at-a-glance channel).
$ErrorActionPreference = 'Stop'
$assetDir = Join-Path 'd:\PureEngine' 'assets'
if (-not (Test-Path $assetDir)) { New-Item -ItemType Directory -Path $assetDir | Out-Null }

$width = 16; $height = 16

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

function Write-Texture([string]$fileName, [int[]]$colorA, [int[]]$colorB) {
    $out = Join-Path $assetDir $fileName
    # 2-tone per-pixel checkerboard — the parity pattern keeps UV sampling
    # provably correct (same idea as the Step 10 checker, finer grain).
    $raw = New-Object byte[] ($height * (1 + $width * 3))
    $idx = 0
    for ($y = 0; $y -lt $height; $y++) {
        $raw[$idx] = 0; $idx++                     # filter: None
        for ($x = 0; $x -lt $width; $x++) {
            if (($x + $y) % 2 -eq 0) { $px = $colorA } else { $px = $colorB }
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
    # Assemble PNG — 8-bit RGB (color type 2), same as the checker
    $signature = [byte[]]@(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A)
    $ihdr = (Get-BE32 $width) + (Get-BE32 $height) + [byte[]]@(8, 2, 0, 0, 0)
    $png = $signature + (Get-Chunk 'IHDR' $ihdr) + (Get-Chunk 'IDAT' $idat) + (Get-Chunk 'IEND' @())
    [System.IO.File]::WriteAllBytes($out, $png)
    Write-Host "Wrote $out ($($png.Length) bytes, ${width}x${height} RGB)"
}

# Palette — warm green / steel blue / crimson (see tint rule above).
Write-Texture 'tex_player.png'  @(120, 190,  90) @(170, 230, 130)
Write-Texture 'tex_scenery.png' @( 80, 120, 180) @(120, 160, 220)
Write-Texture 'tex_hostile.png' @(170,  40,  40) @(220,  90,  60)
