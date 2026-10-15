# Receptor UDP de diagnostico: escucha en un puerto y registra cada datagrama con
# marca de tiempo, tamaño y contenido (caracteres de control escapados), para
# comparar byte a byte lo que envia el traductor con lo que enviaba UDPcli.exe.
#
#   powershell -ExecutionPolicy Bypass -File tests\udp-capture.ps1 -Port 27182 -Out captura.txt
#
# Termina solo tras IdleSeconds sin recibir nada (una vez que llego algo) o con Ctrl+C.
param(
	[int]$Port = 27182,
	[string]$Out = "udp-captura.txt",
	[int]$IdleSeconds = 10
)

$udp = New-Object System.Net.Sockets.UdpClient($Port)
$udp.Client.ReceiveTimeout = 500
$remote = New-Object System.Net.IPEndPoint([System.Net.IPAddress]::Any, 0)
$sw = [System.Diagnostics.Stopwatch]::StartNew()
$lines = @()
$count = 0
$first = $null
$last = $null
$lastRx = $null

function Escape([byte[]]$bytes) {
	$sb = New-Object System.Text.StringBuilder
	foreach ($b in $bytes) {
		if ($b -eq 0x0A) { [void]$sb.Append('\n') }
		elseif ($b -eq 0x0D) { [void]$sb.Append('\r') }
		elseif ($b -eq 0x12) { [void]$sb.Append('<DC2>') }
		elseif ($b -eq 0x14) { [void]$sb.Append('<DC4>') }
		elseif ($b -lt 0x20 -or $b -gt 0x7E) { [void]$sb.AppendFormat('<{0:X2}>', $b) }
		else { [void]$sb.Append([char]$b) }
	}
	$sb.ToString()
}

Write-Host "Escuchando UDP en el puerto $Port (termina tras $IdleSeconds s sin datos)..."
while ($true) {
	try {
		$data = $udp.Receive([ref]$remote)
	} catch [System.Net.Sockets.SocketException] {
		if ($lastRx -ne $null -and ($sw.ElapsedMilliseconds - $lastRx) -gt ($IdleSeconds * 1000)) { break }
		continue
	}
	$t = $sw.ElapsedMilliseconds
	if ($first -eq $null) { $first = $t; $last = $t }
	$count++
	$line = "{0,4}  t={1,7} ms  +{2,5} ms  {3,4} bytes  {4}" -f $count, ($t - $first), ($t - $last), $data.Length, (Escape $data)
	$lines += $line
	Write-Host $line
	$last = $t
	$lastRx = $t
}
$udp.Close()
$lines += ("-- {0} datagramas desde {1} en {2} ms" -f $count, $remote.Address, ($last - $first))
$lines | Set-Content -Path $Out -Encoding UTF8
Write-Host "Guardado en $Out"
