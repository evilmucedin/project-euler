<#
Install the latest Buck2 release on Windows.

Usage:
  powershell -ExecutionPolicy Bypass -File .\installBuck2Windows.ps1
  powershell -ExecutionPolicy Bypass -File .\installBuck2Windows.ps1 -InstallDir C:\Tools\Buck2

Installs buck2.exe to $InstallDir, defaulting to $env:USERPROFILE\.local\bin.
Requires zstd.exe on PATH because Buck2 release assets are distributed as .zst.
#>

param(
    [string]$InstallDir = "$env:USERPROFILE\.local\bin"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command zstd -ErrorAction SilentlyContinue)) {
    Write-Error "zstd.exe is required on PATH. Install zstd, then rerun this script."
}

$arch = (Get-CimInstance Win32_Processor | Select-Object -First 1 -ExpandProperty Architecture)
switch ($arch) {
    9 { $buckArch = "x86_64-pc-windows-msvc.exe" }
    12 { $buckArch = "aarch64-pc-windows-msvc.exe" }
    default { throw "Unsupported Windows CPU architecture code: $arch" }
}

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
$tmp = New-Item -ItemType Directory -Force -Path ([System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), "buck2-install-$([System.Guid]::NewGuid())"))
try {
    $archive = Join-Path $tmp.FullName "buck2.exe.zst"
    $installPath = Join-Path $InstallDir "buck2.exe"
    $url = "https://github.com/facebook/buck2/releases/download/latest/buck2-$buckArch.zst"

    Write-Host ">>> Downloading Buck2 latest ($buckArch)"
    Invoke-WebRequest -Uri $url -OutFile $archive
    & zstd -d -f $archive -o $installPath

    Write-Host ">>> Installed Buck2: $installPath"
    & $installPath --version

    $pathParts = ($env:PATH -split ';') | Where-Object { $_ }
    if ($pathParts -notcontains $InstallDir) {
        Write-Host "NOTE: add $InstallDir to PATH to use buck2.exe without an absolute path."
    }
} finally {
    Remove-Item -Recurse -Force $tmp.FullName -ErrorAction SilentlyContinue
}
