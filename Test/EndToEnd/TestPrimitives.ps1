[CmdletBinding()]
param (
    [Parameter(Mandatory=$false)]
    [string] $ServerPlatform = $env:VC_PLATFORM,
    [Parameter(Mandatory=$false)]
    [string] $ClientPlatform = $env:VC_PLATFORM
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

. .\Mechanisms.ps1

try
{
    $build = 'Debug'
    $testService = 'Test'
    $testTopic = '[Empty.xls]Sheet'
    $testValue = 'Hello, World'

    Write-NetDDEServerConfig $build $ServerPlatform
    $server = Start-NetDDEServer $build $ServerPlatform

    Write-NetDDEClientConfig $build $ClientPlatform Excel $testService
    $client = Start-NetDDEClient $build $ClientPlatform

    $workbookPath = (Resolve-Path 'Empty.xls').Path
    $excel = Start-Excel $workbookPath

    Poke-Value $testService $testTopic R1C1 $testValue

    $result = Request-Value $testService $testTopic R1C1

    if ($result -ne $testValue)
    {
        throw "ERROR: Expected '${testValue}', received '${result}'"
    }

    Execute-Command $testService $testTopic '[Calculate.Now()]'
}
finally
{
    if (Test-Path Variable:\excel) { Stop-Excel $excel }
    if (Test-Path Variable:\client) { Stop-NetDDEClient $client }
    if (Test-Path Variable:\server) { Stop-NetDDEServer $server }
    if (Test-Path Variable:\workbookPath) { git restore $workbookPath }
}
