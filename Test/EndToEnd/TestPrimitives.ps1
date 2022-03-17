Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

. .\Mechanisms.ps1

try
{
$build = 'Debug'
$testService = 'Test'
$testValue = 'Hello, World'

Write-NetDDEServerConfig $build
$server = Start-NetDDEServer $build

Write-NetDDEClientConfig $build Excel $testService
$client = Start-NetDDEClient $build

$workbookPath = (Resolve-Path 'Empty.xls').Path
$excel = Start-Excel $workbookPath

Poke-Value $testService '[Empty.xls]Sheet' R1C1 $testValue

$result = Request-Value $testService '[Empty.xls]Sheet' R1C1

if ($result -ne $testValue)
{
    throw "ERROR: Expected '${testValue}', received '${result}'"
}

Execute-Command $testService '[Empty.xls]Sheet' '[Calculate.Now()]'
}
finally
{
	if (Test-Path Variable:\excel) { Stop-Excel $excel }
	if (Test-Path Variable:\client) { Stop-NetDDEClient $client }
	if (Test-Path Variable:\server) { Stop-NetDDEServer $server }
}
