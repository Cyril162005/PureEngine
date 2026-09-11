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
)
# NOTE: synced with the CMake POST_BUILD arcade bundle (13 files). Deliberately
# excluded: tilemap_default.txt (sample only — no game loads it at runtime),
# stress fixtures, platformer_* (Platformer has its own POST_BUILD bundle),
# paddle_* (Pong has its own bundle). This zip ships PureEngine.exe (arcade).

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

Compress-Archive -Path $stageDir -DestinationPath $zipPath -CompressionLevel Optimal
Write-Output "Created $zipPath"
