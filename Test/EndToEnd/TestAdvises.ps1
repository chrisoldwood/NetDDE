Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

. .\Mechanisms.ps1

try
{
    $build = 'Debug'
    $platform = $env:VC_PLATFORM
    $testService = 'Test'
    $testTopic = '[Formula.xls]Sheet'

    Write-NetDDEServerConfig $build $platform
    $server = Start-NetDDEServer $build $platform

    Write-NetDDEClientConfig $build $platform Excel $testService
    $client = Start-NetDDEClient $build $platform

    $workbookPath = (Resolve-Path 'Formula.xls').Path
    $excel = Start-Excel $workbookPath

    $advisejob = Start-Job { & DDECmd advise --server $using:testService --topic $using:testTopic --item R10C10 }
    $attempt=3;
    while ($attempt-- -gt 0)
    {
        $values = Receive-Job $advisejob
        if ($values | where { $_ -eq '#CONNECTED' }) { break };
        Start-Sleep -Seconds 1
    };
    if ($attempt -lt 0)
    {
        throw 'ERROR: Timed-out waiting for DDE advise loop to be established.'
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
}
