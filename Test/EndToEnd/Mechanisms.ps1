Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

function Exec([scriptblock] $command)
{
	& $command
	if ($LastExitCode -ne 0)
	{
		throw "ERROR: Failed execute '${command}' [${LastExitCode}]"
	}
}

function Test-ExcelRunning([string] $filename)
{
	if (!(Get-Command DDECmd.exe -ErrorAction SilentlyContinue))
	{
		throw 'ERROR: DDECmd is not on the PATH.'
	}

	$instance = & DDECmd servers | where { $_ -like 'Excel|*[$filename]*'}
	if ($LastExitCode -ne 0)
	{
		throw "ERROR: Failed to query for running DDE servers [${LastExitCode}]."
	}

	return ($null -ne $instance)
}

function Start-Excel([string] $workbookPath)
{
	$excelPath = 'C:\Program Files\Microsoft Office\root\Office16\EXCEL.EXE'
	$process = Start-Process $excelPath -ArgumentList $workbookPath -PassThru -WindowStyle Minimized

	$filename = Split-Path -Leaf $workbookPath
	Start-Sleep -Seconds 1

	foreach($attempt in 1..3)
	{
		if (Test-ExcelRunning $filename)
		{
			return $process
		}

		Write-Host "Still waiting for Excel to start..."
		Start-Sleep -Seconds 1
	}

	throw 'ERROR: Excel does not appear to have started.'
}

function Stop-Excel([System.Diagnostics.Process] $process)
{
	Stop-Process $process.Id
}

function Write-NetDDEServerConfig([string] $config)
{
	$iniFile = "..\..\Server\$config\NetDDEServer.ini"
	$config =
@"
[Version]
Version=1.0

[Server]
Port=8888

[Main]
TrayIcon=False
"@

	$config | Out-File -Encoding Ascii $iniFile
}

function Start-NetDDEServer([string] $config)
{
	$path = "..\..\Server\$config\NetDDEServer.exe"
	$process = Start-Process $path -PassThru -WindowStyle Minimized
	return $process
}

function Stop-NetDDEServer([System.Diagnostics.Process] $process)
{
	Stop-Process $process.Id
}

function Write-NetDDEClientConfig([string] $config, [string] $remoteName, [string] $localName)
{
	$iniFile = "..\..\Client\$config\NetDDEClient.ini"
	$config =
@"
[Version]
Version=1.0

[Services]
Count=1
Service[0]=${localName}

[TEST]
RemoteName=${remoteName}
LocalName=${localName}
Server=localhost

[Main]
TrayIcon=False
"@

	$config | Out-File -Encoding Ascii $iniFile
}

function Start-NetDDEClient([string] $config)
{
	$path = "..\..\Client\$config\NetDDEClient.exe"
	$process = Start-Process $path -PassThru -WindowStyle Minimized
	return $process
}

function Stop-NetDDEClient([System.Diagnostics.Process] $process)
{
	Stop-Process $process.Id
}

function Request-Value([string] $service, [string] $topic, [string] $item)
{
	return Exec { & DDECmd request --server $service --topic $topic --item $item }
}

function Poke-Value([string] $service, [string] $topic, [string] $item, [string] $value)
{
	Exec { & DDECmd poke --server $testService --topic $topic --item $item --value $value }
}

function Execute-Command([string] $service, [string] $topic, [string] $cmd)
{
	Exec { & DDECmd execute --server $service --topic $topic --command $cmd }
}
