# make_music.ps1 - Step 123: generates assets/music_loop.wav
# Same hand-written binary machinery as make_font.ps1 / make_textures.ps1.
# Format: RIFF WAV, PCM 16-bit, mono, 22050 Hz, 4.0 seconds (~176 KB).
# The three sine partials use frequencies that are integer multiples of
# 1/duration (110/165/220 Hz over 4 s), so every partial completes whole
# cycles at the loop point - the file loops seamlessly with no click.

$ErrorActionPreference = "Stop"
$outPath = Join-Path $PSScriptRoot "assets\music_loop.wav"

$sampleRate = 22050
$duration   = 4.0
$numSamples = [int]($sampleRate * $duration)
$dataBytes  = $numSamples * 2          # 16-bit mono

# --- RIFF header (44 bytes) ---
$ms = New-Object System.IO.MemoryStream
$bw = New-Object System.IO.BinaryWriter($ms)

$bw.Write([byte[]][char[]]"RIFF")
$bw.Write([int](36 + $dataBytes))      # riff chunk size
$bw.Write([byte[]][char[]]"WAVE")
$bw.Write([byte[]][char[]]"fmt ")
$bw.Write([int]16)                     # fmt chunk size
$bw.Write([int16]1)                    # PCM
$bw.Write([int16]1)                    # mono
$bw.Write([int]$sampleRate)
$bw.Write([int]($sampleRate * 2))      # byte rate
$bw.Write([int16]2)                    # block align
$bw.Write([int16]16)                   # bits per sample
$bw.Write([byte[]][char[]]"data")
$bw.Write([int]$dataBytes)

# --- Samples: quiet ambient chord, seamless loop ---
# 110 Hz (A2), 165 Hz, 220 Hz - all integer multiples of 0.25 Hz.
for ($i = 0; $i -lt $numSamples; $i++) {
    $t = $i / $sampleRate
    $v = 0.12 * [Math]::Sin(2 * [Math]::PI * 110 * $t) +
         0.08 * [Math]::Sin(2 * [Math]::PI * 165 * $t) +
         0.05 * [Math]::Sin(2 * [Math]::PI * 220 * $t)
    $bw.Write([int16][Math]::Round($v * 32767))
}

$bw.Flush()
[System.IO.File]::WriteAllBytes($outPath, $ms.ToArray())
$bw.Close()
$ms.Close()

$written = Get-Item $outPath
Write-Output ("music_loop.wav written: {0} bytes ({1} s, {2} Hz, 16-bit mono)" -f $written.Length, $duration, $sampleRate)
