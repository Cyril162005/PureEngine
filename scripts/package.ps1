$version = "0.1.0"

$repoRoot = Split-Path -Parent $PSScriptRoot
$releaseDir = Join-Path $repoRoot "build\Release"
$packageName = "PureEngine-$version-win64"
$packageRoot = Join-Path $repoRoot "package"
$stageDir = Join-Path $packageRoot $packageName
$zipPath = Join-Path $packageRoot "$packageName.zip"
$executablePath = Join-Path $releaseDir "PureEngine.exe"

$runtimeAssets = @(
    "beep.wav"
    "GAMEOVER.wav"
    "win_sound.wav"
    "checker.png"
    "font_digits.png"
    "tex_player.png"
    "tex_scenery.png"
    "tex_hostile.png"
    "tex_hostile_alt.png"
    "hostile_default.txt"
    "hostile_alt.txt"
    "animation_default.txt"
    "arcade_arena.txt"
    "paddle_spritesheet.png"
)
# Single source of truth for the shipped arcade zip (F-08). CMake POST_BUILD
# arcade bundle mirrors this list for dev runs (14 files: renderer's fixed
# 7-texture set requires paddle_spritesheet.png even for arcade). Deliberately
# excluded: tilemap_default.txt (sample), stress fixtures, platformer_* (own
# bundle), paddle_animations.txt (Pong-only). This zip ships PureEngine.exe.

if (-not (Test-Path $executablePath -PathType Leaf)) {
    throw "Release executable not found: $executablePath"
}

if (Test-Path $packageRoot) {
    Remove-Item $packageRoot -Recurse -Force
}

$stageAssetsDir = Join-Path $stageDir "assets"
New-Item -ItemType Directory -Path $stageAssetsDir -Force | Out-Null
Copy-Item $executablePath (Join-Path $stageDir "PureEngine.exe")

foreach ($asset in $runtimeAssets) {
    $sourcePath = Join-Path $repoRoot (Join-Path "assets" $asset)
    if (-not (Test-Path $sourcePath -PathType Leaf)) {
        throw "Runtime asset not found: $sourcePath"
    }
    Copy-Item $sourcePath (Join-Path $stageAssetsDir $asset)
}

# Step 77: the renderer loads GLSL from files — the zip must carry
# assets/shaders/ or the packaged game exits at init (same class as the
# paddle-sheet find: fatal renderer asset missing from a bundle).
$shaderSourceDir = Join-Path $repoRoot (Join-Path "assets" "shaders")
if (-not (Test-Path $shaderSourceDir -PathType Container)) {
    throw "Runtime shader dir not found: $shaderSourceDir"
}
Copy-Item $shaderSourceDir (Join-Path $stageAssetsDir "shaders") -Recurse

Compress-Archive -Path $stageDir -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output "Created $zipPath"
