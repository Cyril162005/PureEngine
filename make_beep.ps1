# Generates assets/beep.wav: 0.15s 880Hz sine, 44100Hz 16-bit mono, with a
# 5ms fade-in / 20ms fade-out envelope so the beep has no click artifacts.
$ErrorActionPreference = 'Stop'
$assetDir = Join-Path 'd:\PureEngine' 'assets'
if (-not (Test-Path $assetDir)) { New-Item -ItemType Directory -Path $assetDir | Out-Null }
$out = Join-Path $assetDir 'beep.wav'

$sampleRate = 44100
$duration   = 0.15
$freq       = 880.0
$amplitude  = 0.6
$numSamples = [int]($sampleRate * $duration)
$fadeIn     = [int]($sampleRate * 0.005)   # 5 ms
$fadeOut    = [int]($sampleRate * 0.020)   # 20 ms

$fs = [System.IO.File]::Open($out, [System.IO.FileMode]::Create)
$bw = New-Object System.IO.BinaryWriter($fs)

# --- WAV header (RIFF/WAVE, PCM, mono, 16-bit) ---
$dataSize  = $numSamples * 2                 # 2 bytes per sample
$chunkSize = 36 + $dataSize
$bw.Write([System.Text.Encoding]::ASCII.GetBytes('RIFF'))
$bw.Write([uint32]$chunkSize)
$bw.Write([System.Text.Encoding]::ASCII.GetBytes('WAVE'))
$bw.Write([System.Text.Encoding]::ASCII.GetBytes('fmt '))
$bw.Write([uint32]16)                        # fmt chunk size for PCM
$bw.Write([uint16]1)                         # audio format 1 = PCM
$bw.Write([uint16]1)                         # channels: mono
$bw.Write([uint32]$sampleRate)
$bw.Write([uint32]($sampleRate * 2))         # byte rate = sampleRate * blockAlign
$bw.Write([uint16]2)                         # block align = channels * bits/8
$bw.Write([uint16]16)                        # bits per sample
$bw.Write([System.Text.Encoding]::ASCII.GetBytes('data'))
$bw.Write([uint32]$dataSize)

# --- samples ---
for ($i = 0; $i -lt $numSamples; $i++) {
    $env = 1.0
    if ($i -lt $fadeIn) { $env = $i / [double]$fadeIn }
    $remaining = $numSamples - $i
    if ($remaining -lt $fadeOut) { $env = [Math]::Min($env, $remaining / [double]$fadeOut) }
    $sample = [Math]::Sin(2.0 * [Math]::PI * $freq * $i / $sampleRate) * $amplitude * $env
    $bw.Write([int16]([Math]::Round($sample * 32767.0)))
}

$bw.Flush()
$bw.Dispose()
Write-Host "Wrote $out ($numSamples samples, $($duration * 1000) ms, $($dataSize + 44) bytes total)"
