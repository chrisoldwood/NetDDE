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
    $testTopic = '[Formula.xls]Sheet'

    Write-NetDDEServerConfig $build $ServerPlatform
    $server = Start-NetDDEServer $build $ServerPlatform

    Write-NetDDEClientConfig $build $ClientPlatform Excel $testService
    $client = Start-NetDDEClient $build $ClientPlatform

    $workbookPath = (Resolve-Path 'Formula.xls').Path
    $excel = Start-Excel $workbookPath

    $advisejob = Start-Job { & DDECmd advise --server $using:testService --topic $using:testTopic --item R10C10 }
    $attempt=5;
    while ($attempt-- -gt 0)
    {
        $values = Receive-Job $advisejob
        if ($values | where { $_ -eq '#CONNECTED' }) { break };
        Start-Sleep -Seconds 1
    };
    if ($attempt -lt 0)
    {
        Write-Warning 'Timed-out waiting for evidence (#CONNECTED) that DDE advise loop was established.'
    }
    $values = @()

    1..9 | foreach { Poke-Value $testService $testTopic R1C1 $_ }

    $attempt=3;
    while ($attempt-- -gt 0)
    {
        $values += Receive-Job $advisejob
        if ($values.Count -ge 9) { break };
        Start-Sleep -Seconds 1
    };
    if ($attempt -lt 0)
    {
        throw 'ERROR: Timed-out waiting for DDE link to be advised enough times.'
    }

    if ($values.Count -ne 9)
    {
        throw "ERROR: Unexpected number of advises: $($values.Count)"
    }

    if ( ($values[0] -ne 100) -and ($values[8]) -ne 108)
    {
        $sequence = $values -join ','
        throw "ERROR: Unexpected sequence of advised values: ${sequence}"
    }
}
finally
{
    if (Test-Path Variable:\advisejob) { Stop-Job -ErrorAction Continue $advisejob }
    if (Test-Path Variable:\excel) { Stop-Excel $excel }
    if (Test-Path Variable:\client) { Stop-NetDDEClient $client }
    if (Test-Path Variable:\server) { Stop-NetDDEServer $server }
    if (Test-Path Variable:\workbookPath) { git restore $workbookPath }
}
