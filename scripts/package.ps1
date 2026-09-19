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
    "music_loop.wav"
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
    "paddle_animations.txt"
    "platformer_step72_proof.txt"
    "input_bindings.txt"
)
# Single source of truth for the shipped arcade zip (F-08 re-fix). CMake POST_BUILD
# arcade bundle mirrors this list for dev runs (18 files + prefabs/ + shaders:
# renderer's fixed 7-texture set requires paddle_* even for arcade; platformer
# proof ships for verification). Deliberately excluded: tilemap_default.txt
# (sample), stress fixtures, platformer_level*.txt (Platformer own bundle).
# This zip ships PureEngine.exe.

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

# Step 120/138: prefab templates — the spawn_prefab console command
# probes assets/prefabs/; the zip must carry the directory or the
# packaged game's spawn path fails (same class as the shader find).
$prefabSourceDir = Join-Path $repoRoot (Join-Path "assets" "prefabs")
if (-not (Test-Path $prefabSourceDir -PathType Container)) {
    throw "Runtime prefab dir not found: $prefabSourceDir"
}
Copy-Item $prefabSourceDir (Join-Path $stageAssetsDir "prefabs") -Recurse

# Step 143: dry-run / verification checklist (how to confirm this script):
#   1. Build the exe first: cmake --build build --config Release
#   2. Run:  powershell -ExecutionPolicy Bypass -File scripts\package.ps1
#      -> prints "Created package\PureEngine-0.1.0-win64.zip"
#   3. Confirm the zip contents (Step 138 verification):
#        Add-Type -AssemblyName System.IO.Compression.FileSystem
#        [System.IO.Compression.ZipFile]::OpenRead("$pwd\package\PureEngine-0.1.0-win64.zip").Entries.FullName
#      Expected: 24 entries — PureEngine.exe + 18 runtime assets
#      (MUST include music_loop.wav and input_bindings.txt) +
#      prefabs/enemy.txt + shaders/ (4 files).
#   Missing prefabs/ or music_loop.wav = the packaged game's spawn_prefab
#   or music path breaks (the exact drift Step 138 fixed).

Compress-Archive -Path $stageDir -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output "Created $zipPath"
