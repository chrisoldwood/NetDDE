Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

. .\Mechanisms.ps1

try
{
    $build = 'Debug'

    Write-NetDDEServerConfig $build
    $server = Start-NetDDEServer $build

    $workbookPath = (Resolve-Path 'Empty.xls').Path
    $excel = Start-Excel $workbookPath

    Push-Location netdde-js
    cmd.exe /c "call run.bat"
    Pop-Location
    if ($LastExitCode -ne 0) { exit $LastExitCode }
}
finally
{
    if (Test-Path Variable:\excel) { Stop-Excel $excel }
    if (Test-Path Variable:\server) { Stop-NetDDEServer $server }
}
