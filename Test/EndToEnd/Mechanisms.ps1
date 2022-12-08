Set-StrictMode -Version Latest
$ErrorActionPreference = 'stop'

function Validate-Build([string] $value)
{
	if ( ($value -notlike 'debug') -and ($value -notlike 'release') )
	{
		throw "ERROR: Invalid build type '${value}'"
	}
}

function Validate-Platform([string] $value)
{
	if ( ($value -notlike 'win32') -and ($value -notlike 'x64') )
	{
		throw "ERROR: Invalid platform type '${value}'"
	}
}

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
	$process = Start-Process $excelPath -ArgumentList $workbookPath -PassThru

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
	Stop-Process -ErrorAction Continue $process.Id
}

function Write-NetDDEServerConfig([string] $build, [string] $platform)
{
	Validate-Build $build
	Validate-Platform $platform

	$iniFile = "..\..\Server\${build}\${platform}\NetDDEServer.ini"
	$config =
@"
[Version]
Version=1.0

[Server]
Port=8888

[Main]
TrayIcon=False
DDETimeOut=1000
"@

	$config | Out-File -Encoding Ascii $iniFile
}

function Start-NetDDEServer([string] $build, [string] $platform)
{
	Validate-Build $build
	Validate-Platform $platform

	$path = "..\..\Server\${build}\${platform}\NetDDEServer.exe"
	$process = Start-Process $path -PassThru -WindowStyle Minimized
	return $process
}

function Stop-NetDDEServer([System.Diagnostics.Process] $process)
{
	Stop-Process -ErrorAction Continue $process.Id
}

function Write-NetDDEClientConfig([string] $build, [string] $platform, [string] $remoteName, [string] $localName)
{
	Validate-Build $build
	Validate-Platform $platform

	$iniFile = "..\..\Client\${build}\${platform}\NetDDEClient.ini"
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
InitialValue=#CONNECTED

[Main]
TrayIcon=False
NetTimeOut=1000
"@

	$config | Out-File -Encoding Ascii $iniFile
}

function Start-NetDDEClient([string] $build, [string] $platform)
{
	Validate-Build $build
	Validate-Platform $platform

	$path = "..\..\Client\${build}\${platform}\NetDDEClient.exe"
	$process = Start-Process $path -PassThru -WindowStyle Minimized
	return $process
}

function Stop-NetDDEClient([System.Diagnostics.Process] $process)
{
	Stop-Process -ErrorAction Continue $process.Id
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
