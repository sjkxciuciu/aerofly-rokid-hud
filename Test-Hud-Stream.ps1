# Standalone demo sender. Stop Aerofly first: this uses the same discovery port.
param([string]$Target = '255.255.255.255', [int]$Seconds = 60)
$ErrorActionPreference = 'Stop'
$udp = [System.Net.Sockets.UdpClient]::new(49003)
$udp.EnableBroadcast = $true
$udp.Client.Blocking = $false
$destination = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Parse($Target), 49002)
$timer = [Diagnostics.Stopwatch]::StartNew()
$session = [DateTimeOffset]::UtcNow.ToUnixTimeMilliseconds()
$sequence = 0
Write-Host 'Sending TEST telemetry; HUD will show 测试数据. Ctrl+C stops the demo.'
try {
  while ($timer.Elapsed.TotalSeconds -lt $Seconds) {
    while ($udp.Available -gt 0) {
      $source = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)
      $hello = $udp.Receive([ref]$source)
      if ([Text.Encoding]::ASCII.GetString($hello) -eq 'AEROFLY_ROKID_HELLO_V1') {
        $destination = [System.Net.IPEndPoint]::new($source.Address, 49002)
      }
    }
    $t = $timer.Elapsed.TotalSeconds
    $sequence++
    $packet = [ordered]@{
      v = 2; session = $session; seq = $sequence; flight = $true; test = $true
      mode = if ($destination.Address.Equals([System.Net.IPAddress]::Broadcast)) { 'broadcast' } else { 'unicast' }
      ias = 152 + 10 * [Math]::Sin($t * .2); alt = 3500 + 250 * [Math]::Sin($t * .15)
      vs = 2250 * [Math]::Cos($t * .15); gear = ([Math]::Sin($t * .25) + 1) / 2
      pitch = 4 * [Math]::Sin($t * .45); bank = 25 * [Math]::Sin($t * .32)
      hdg = ($t * 3) % 360; thr = 0.68
      flaps = 0.33; spoilers = 0; master = 0; caution = 0; fire = 0; oil = 0; fuel = 0; hyd = 0; altAlert = 0
    } | ConvertTo-Json -Compress
    $bytes = [Text.Encoding]::UTF8.GetBytes($packet)
    [void]$udp.Send($bytes, $bytes.Length, $destination)
    Start-Sleep -Milliseconds 50
  }
} finally { $udp.Dispose() }
