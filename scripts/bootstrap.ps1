# Resolve paths
$ProjectRoot = Split-Path -Parent $PSScriptRoot

# Check if this is a git repo
if (Test-Path (Join-Path $ProjectRoot ".git") -PathType Container) {
    git -C "$ProjectRoot" submodule update --init --recursive
} else {
    $vcpkgPath = Join-Path $ProjectRoot "vcpkg"
    git clone https://github.com/microsoft/vcpkg "$vcpkgPath"
}

# Bootstrap vcpkg
& "$ProjectRoot\vcpkg\bootstrap-vcpkg.bat"
