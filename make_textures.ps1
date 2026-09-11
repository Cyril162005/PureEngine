# Game Build Phase 5: generates RGB PNGs in assets/ —
#   tex_player.png   64x64 warm-green ship  - entity 0 (you)
#   tex_scenery.png  16x16 steel blue       - entities 1..2 (the scenery pair)
#   tex_hostile.png  16x16 crimson          - entities 3..5 (the threats)
# Same hand-written PNG machinery as make_checker.ps1: signature, IHDR,
# zlib-wrapped deflate IDAT, IEND — big-endian fields, CRC32 per chunk,
# filter byte 0 per scanline. Every pixel is data in THIS script, so the
# textures are reproducible byte-for-byte with no art tools.
# The loader (resources.h) decodes ANY resolution via stb_image and uploads
# the decoded w/h — nothing in the engine assumes 16x16 (the 256x32 paddle
# sheet already renders). The player sprite is 64x64 to prove real-art
# loading; the world triangle samples UVs 0..1 so it renders at entity
# scale with 4x the detail and no code changes.
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

function Write-Texture([string]$fileName, [int]$width, [int]$height, [int[]]$colorA, [int[]]$colorB) {
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

function Write-Ship([string]$fileName, [int]$size) {
    # 64x64 player ship: upward triangle hull with dark outline, vertical
    # gradient body, cockpit dot. Procedural (no pixel tables): edge
    # functions of (x,y), same repo discipline as the paddle-sheet hues.
    # All channels keep red content (tint rule above). The world triangle
    # samples UV (0,0),(1,0),(.5,1), so art concentrates upper-center, but
    # the full canvas is painted so any sampled region reads as ship.
    $out = Join-Path $assetDir $fileName
    $half = $size / 2
    $raw = New-Object byte[] ($size * (1 + $size * 3))
    $idx = 0
    for ($y = 0; $y -lt $size; $y++) {
        $raw[$idx] = 0; $idx++                     # filter: None
        $v = $y / ($size - 1)                      # 0 top .. 1 bottom
        for ($x = 0; $x -lt $size; $x++) {
            $u = $x / ($size - 1)                  # 0 left .. 1 right
            # Hull: upward triangle, apex top-center, base near bottom.
            $halfWidthAtY = 0.08 + 0.34 * $v
            $dx = [math]::Abs($u - 0.5)
            $inHull = $dx -le $halfWidthAtY
            $onEdge = $inHull -and ($dx -gt ($halfWidthAtY - 0.035))
            # Cockpit: small disc upper-middle.
            $cdx = $u - 0.5; $cdy = ($v - 0.32) * 1.2
            $inCockpit = ($cdx * $cdx + $cdy * $cdy) -le 0.004
            if ($inCockpit) {
                $px = @(200, 180, 80)
            } elseif ($onEdge) {
                $px = @(40, 80, 40)
            } elseif ($inHull) {
                # Body gradient: light top -> deep bottom, warm green.
                $t = $v
                $px = @([int](120 + 50 * (1 - $t)), [int](190 - 60 * $t), [int](90 - 20 * $t))
            } else {
                # Backdrop: dark green gradient (fills unsampled corners too).
                $px = @([int](44 - 12 * $v), [int](96 - 32 * $v), [int](48 - 12 * $v))
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
    $ihdr = (Get-BE32 $size) + (Get-BE32 $size) + [byte[]]@(8, 2, 0, 0, 0)
    $png = $signature + (Get-Chunk 'IHDR' $ihdr) + (Get-Chunk 'IDAT' $idat) + (Get-Chunk 'IEND' @())
    [System.IO.File]::WriteAllBytes($out, $png)
    Write-Host "Wrote $out ($($png.Length) bytes, ${size}x${size} RGB ship)"
}

# Palette — warm green / steel blue / crimson (see tint rule above).
Write-Ship 'tex_player.png' 64
Write-Texture 'tex_scenery.png' 16 16 @( 80, 120, 180) @(120, 160, 220)
Write-Texture 'tex_hostile.png' 16 16 @(170,  40,  40) @(220,  90,  60)
